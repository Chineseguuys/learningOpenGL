#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexcoords;
layout (location = 2) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;
out vec2 texcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    texcoords = aTexcoords;
    Normal = mat3(transpose(inverse(model)))  * aNormal;
    Position = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}