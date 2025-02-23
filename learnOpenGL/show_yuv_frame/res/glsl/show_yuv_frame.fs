#version 330 core

#define YUV_MAXIMUM 255.0
#define BT601 0
#define BT709 1

in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D texY;
uniform sampler2D texU;
uniform sampler2D texV;

uniform int whichMatrix;
uniform bool swapUV;

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

    // YUV 归一化（假设输入范围是 16-235）TV Range
    vec3 yuv = vec3(y, u, v) - vec3(16.0 / YUV_MAXIMUM, 128.0 / YUV_MAXIMUM, 128.0 / YUV_MAXIMUM);

    // 矩阵转换
    vec3 rgb;
    if (whichMatrix == BT601) {
        rgb.r = 1.164 * yuv.r + 1.596 * yuv.b;
        rgb.g = 1.164 * yuv.r - 0.392 * yuv.g - 0.813 * yuv.b;
        rgb.b = 1.164 * yuv.r + 2.017 * yuv.g;
    }else if (whichMatrix == BT709) {
        rgb.r = 1.164 * yuv.r + 1.793 * yuv.b;
        rgb.g = 1.164 * yuv.r - 0.213 * yuv.g - 0.533 * yuv.b;
        rgb.b = 1.164 * yuv.r + 2.112 * yuv.g;
    } else {
        rgb.r = 1.164 * yuv.r + 1.596 * yuv.b;
        rgb.g = 1.164 * yuv.r - 0.392 * yuv.g - 0.813 * yuv.b;
        rgb.b = 1.164 * yuv.r + 2.017 * yuv.g;
    }
    FragColor = vec4(rgb, 1.0);
}