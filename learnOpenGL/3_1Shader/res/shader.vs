#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec4 vertexColor;

uniform float xShift;

void main()
{
    gl_Position = vec4(aPos.x + xShift, aPos.y, aPos.z, 1.0);
    vertexColor = vec4(aColor.x, aColor.y, aColor.z, 1.0f);
}