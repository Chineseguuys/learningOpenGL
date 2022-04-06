#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 ourColor;

void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
    ourColor = color;
};

#shader fragment
#version 330 core

in vec3 ourColor;
out vec4 color;

void main()
{
    color = vec4(ourColor.x, ourColor.y, ourColor.z, 0.5f);
};