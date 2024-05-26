#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 texcoords;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform sampler2D texture_diffuse1;

void main()
{    
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    //FragColor = vec4(textureCube(skybox, R).rgb, 1.0);
    //FragColor = vec4(texture(skybox, R).rgb, 1.0);
    FragColor = texture(texture_diffuse1, texcoords);
}