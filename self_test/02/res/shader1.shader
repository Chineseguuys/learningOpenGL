#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec3 u_color;

void main()
{
    color = vec4(u_color.x, u_color.y, u_color.z, 1.0f);
};