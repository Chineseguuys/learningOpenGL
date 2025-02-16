#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 LightPos;
    vec3 ViewPos;
    mat3 TBN;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.FragPos = vec3(model * vec4(position, 1.0));
    vs_out.TexCoords = texCoords;

    // mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(tangent);
    vec3 B = normalize(bitangent);
    vec3 N = normalize(normal);

    vs_out.TBN = transpose(mat3(T, B, N));
    vs_out.LightPos = lightPos;
    vs_out.ViewPos = viewPos;
}