#version 330 core
out vec4 FragColor;

in vec4 vertexColor;
in vec2 TexCoord;

uniform sampler2D outTexture;

void main()
{
    // FragColor = texture(outTexture, TexCoord) * vertexColor;
    FragColor = texture(outTexture, TexCoord);
}