#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT {
    vec3 FragPos;
    vec3 Normals;
    vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoord;
    vs_out.Normals = transpose(inverse(mat3(model))) * (-1.0 * aNormal);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}