#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normals;
    vec2 TexCoords;
} vs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform bool useNormalMap;

vec3 normals;

void main()
{
    vec3 color = texture(diffuseMap, vs_in.TexCoords).rgb;
    if (useNormalMap)
        normals = texture(normalMap, vs_in.TexCoords).rgb;
    else
        normals = vs_in.Normals;

    normals = normalize(normals * 2.0 - 1.0);

    vec3 ambient = 0.3 * lightColor;
    vec3 lightDir = normalize(lightPos - vs_in.FragPos);
    float diff = max(dot(lightDir, normals), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - vs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normals);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normals, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    vec3 lighting = (ambient + diffuse + specular) * color;

    FragColor = vec4(lighting, 1.0);
}