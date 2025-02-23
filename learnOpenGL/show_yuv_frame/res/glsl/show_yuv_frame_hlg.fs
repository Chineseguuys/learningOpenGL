#version 330 core

#define YUV_MAXIMUM 255.0
#define BT601 0
#define BT709 1

// HLG 参数
const float HLG_A = 0.17883277;
const float HLG_B = 0.28466892;
const float HLG_C = 0.55991073;
const float HLG_REF_WHITE = 1000.0;  // 参考亮度 (nit)
const float BINARY_16BIT_MAX = 65535.0;

// SDR 参数
const float SDR_MAX_NITS = 100.0;    // SDR 峰值亮度（通常 80-100 nit）
const float GAMMA = 2.2;             // SDR Gamma 值

// 色域转换矩阵：BT.2020 → BT.709
const mat3 BT2020_TO_BT709 = mat3(
    1.6605, -0.5876, -0.0728,
    -0.1246, 1.1329, -0.0083,
    -0.0182, -0.1006, 1.1187
);

in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D texY;
uniform sampler2D texU;
uniform sampler2D texV;

uniform int whichMatrix;
uniform bool swapUV;
uniform bool transposeYuv2Rgb;
uniform bool nonlinear2Linear;

const mat3 yuv2rgb_bt601 = mat3(
    1.164,  0.000,  1.596,
    1.164, -0.392, -0.813,
    1.164,  2.017,  0.000
);

const mat3 yuv2rgb_bt709 = mat3(
    1.164,  0.000,  1.793,
    1.164, -0.213, -0.533,
    1.164,  2.112,  0.000
);

const mat3 yuv2rgb_bt2020nc = mat3(
    1.0, 0.0, 1.4746,
    1.0, -0.164553, -0.571353,
    1.0, 1.8814, 0.0
);

vec3 eotf(vec3 rgb_nonlinear) {
    vec3 rgb_linear;
    for (int i = 0; i < 3; i++) {
        float v = rgb_nonlinear[i];
        if (v <= 0.5) {
            rgb_linear[i] = (v * v) / 3.0;
        } else {
            rgb_linear[i] = (exp((v - HLG_C) / HLG_A) + HLG_B) / 12.0;
        }
    }

    return rgb_linear;
}

// 简单色调映射（Reinhard 算法）
vec3 toneMapReinhard(vec3 hdr) {
    return hdr / (hdr + 1.0);  // 将 HDR 压缩到 [0,1]
}

// HLG YUV → BT.2020nc 矩阵 → 逆 HLG OETF → 线性 HDR RGB → 色调映射 → 色域映射 → Gamma 编码 → SDR 输出
void main() {
    float y, u, v;
    y = texture(texY, texCoord).r;
    if (swapUV) {
        u = texture(texV, texCoord).r;
        v = texture(texU, texCoord).r;
    } else {
        u = texture(texU, texCoord).r;
        v = texture(texV, texCoord).r;
    }

    // reflect value to raw value, value has been normalized to raw_value / BINARY_16BIT_MAX
    // yuv 数据中，y 的数据在 [0, 1] 之间; u, v 的数据在 [-0.5, 0.5] 之间
    // yuv 的数据在传入 gpu 后，被做了归一化的处理
    vec3 yuv = vec3(y, u, v) * BINARY_16BIT_MAX / HLG_REF_WHITE - vec3(0.0, 0.5, 0.5);    // range [0, 1000] -> [0.0, 1.0]

    // BT2020nc transfer matrix
    vec3 rgb;
    rgb = transpose(yuv2rgb_bt2020nc) * yuv;

    rgb = eotf(rgb);

    // nonlinear -> linear
    vec3 linearRgb = rgb;

    // bt2020 to BT709
    vec3 sRgb = transpose(BT2020_TO_BT709) * linearRgb;
    // gamma
    sRgb = pow(sRgb, vec3(1.0 / GAMMA));

    FragColor = vec4(sRgb, 1.0);
}