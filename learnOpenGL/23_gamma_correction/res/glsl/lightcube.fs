#version 430 core
out vec4 FragColor;

uniform vec3 lightCubeColor;

void main()
{
    FragColor = vec4(lightCubeColor, 1.0f); // set alle 4 vector values to 1.0
}