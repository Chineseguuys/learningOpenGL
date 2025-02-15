#version 330 core
out vec4 FragColor;

uniform vec2 upper_left;
uniform vec2 width_height;
uniform float corner_radius;

uniform vec4 forground_color;
// from https://www.shadertoy.com/view/WtdSDs#
// example: https://www.reddit.com/r/opengl/comments/sbrykq/how_to_improve_this_fragment_shader_to_round_rect/?rdt=42399
float roundedBoxSDF(vec2 center, vec2 size, float radius) {
    return length(max(abs(center) - size + radius, 0.0)) - radius;
}


void main()
{
    float edgeSoftness = 1.0;

    vec2 lower_left = vec2(upper_left.x, upper_left.y - width_height.y);

    // gl_FragCoord 表示 窗口坐标系中的实际像素位置
    float distance = roundedBoxSDF(gl_FragCoord.xy - lower_left - (width_height / 2.0f), width_height / 2.0f, corner_radius);
    float smoothedAlpha = 1.0f - smoothstep(0.0f, edgeSoftness, distance);
    FragColor = vec4(1.0, 0.0, 0.0, smoothedAlpha);
}