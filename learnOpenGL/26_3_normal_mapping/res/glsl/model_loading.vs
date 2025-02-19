#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;   // 片段的世界空间的位置
    vec2 TexCoords; // 纹理的坐标
    vec3 LightPos;   // 光源的位置 (切线空间)
    vec3 ViewPos;    // 观察位置(切线空间)
    mat3 TBN;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;

    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = mat3(T, B, N);
    vs_out.TBN = TBN;

    vs_out.LightPos = lightPos;
    vs_out.ViewPos = viewPos;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}