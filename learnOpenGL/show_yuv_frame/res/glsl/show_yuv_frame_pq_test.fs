#version 330 core

#define YUV_MAXIMUM 255.0
#define BT601 0
#define BT709 1

#define USE_PQ_EOTF 0
#define HDR_TO_SDR 0

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

const float PQ_M1 = 0.159301757;
const float PQ_M2 = 78.84375;
const float PQ_C1 = 0.8359375;
const float PQ_C2 = 18.8515625;
const float PQ_C3 = 18.6875;

// 色域转换矩阵：BT.2020 → BT.709
const mat3 BT2020_TO_BT709 = mat3(
    1.6605, -0.5876, -0.0728,
    -0.1246, 1.1329, -0.0083,
    -0.0182, -0.1006, 1.1187
);

const float GAMMA = 2.2;             // SDR Gamma 值

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

const mat3 colorMatrix = mat3(
    1.16438, 0.00, 1.67867,
    1.16438, -0.18733, -0.65042,
    1.16438, 2.14177, 0.00
);

const vec3 colorMatrix_c = vec3(-0.91569, 0.34746, -1.14815);


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

    vec3 yuv = vec3(y, u, v) ;

    vec3 color = yuv * vec3(64.2500, 64.2500, 64.2500);

    vec3 rgb;
    rgb = transpose(colorMatrix) * color + colorMatrix_c;

#if USE_PQ_EOTF
    rgb = eotf_pq(rgb);
    rgb = ootf_pq(rgb);
#endif

#if HDR_TO_SDR
    rgb = pow(rgb, vec3(1.0 / GAMMA));
#endif

    FragColor = vec4(rgb, 1.0);
}