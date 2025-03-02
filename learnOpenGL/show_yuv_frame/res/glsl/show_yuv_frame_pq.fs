#version 330 core

#define YUV_MAXIMUM 255.0
#define BT601 0
#define BT709 1

const float BINARY_16BIT_MAX = 65535.0;

// PQ EOTF 参数
const float PQ_M1 = 0.159301757;
const float PQ_M2 = 78.84375;
const float PQ_C1 = 0.8359375;
const float PQ_C2 = 18.8515625;
const float PQ_C3 = 18.6875;
const float PQ_MAX_REF_WHITE = 10000.0;

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

const mat3 LMS_MATRIX = mat3(
    3.43661, -0.79133, -0.0259499,
    -2.50645,  1.98360, -0.0989137,
    0.06984, -0.192271, 1.12486
);

vec3 eotf_pq(vec3 rgb_nonlinear) {
    vec3 V = clamp(rgb_nonlinear, 0.0, 1.0);
    vec3 V_m2 = pow(V, vec3(1.0 / PQ_M2));
    vec3 numerator = max(V_m2 - PQ_C1, 0.0);
    vec3 denominator = PQ_C2 - PQ_C3 * V_m2;
    vec3 rgb_linear = pow(numerator / denominator, vec3(1.0 / PQ_M1));
    return rgb_linear;
}

vec3 ootf_pq(vec3 rgb_linear) {
    vec3 displayRgb;
    displayRgb.r = (rgb_linear.r < 0.0003024 ) ? (267.84 * rgb_linear.r) : (1.099  * pow(59.5208 * rgb_linear.r, 0.45) - 0.099);
    displayRgb.g = (rgb_linear.g < 0.0003024 ) ? (267.84 * rgb_linear.g) : (1.099  * pow(59.5208 * rgb_linear.g, 0.45) - 0.099);
    displayRgb.b = (rgb_linear.b < 0.0003024 ) ? (267.84 * rgb_linear.b) : (1.099  * pow(59.5208 * rgb_linear.b, 0.45) - 0.099);

    displayRgb = pow(displayRgb, vec3(2.4));
    return displayRgb;
}

vec3 acesTonemap(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}


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
    vec3 yuv = vec3(y, u, v) * BINARY_16BIT_MAX / 1000.0 - vec3(0.0, 0.5, 0.5);    // range [0, 1000] -> [0.0, 1.0]

    // BT2020nc transfer matrix
    vec3 rgb;
    rgb = transpose(yuv2rgb_bt2020nc) * yuv;

    rgb = eotf_pq(rgb);

    //rgb = ootf_pq(rgb);

    // bt2020 to BT709
    vec3 sRgb = transpose(BT2020_TO_BT709) * rgb;
    // gamma
    sRgb = pow(sRgb, vec3(1.0 / GAMMA));

    FragColor = vec4(sRgb, 1.0);
}