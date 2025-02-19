#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;   // 片段的世界空间的位置
    vec2 TexCoords; // 纹理的坐标
    vec3 LightPos;   // 光源的位置 (切线空间)
    vec3 ViewPos;    // 观察位置(切线空间)
    mat3 TBN;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

void main()
{
    // Obtain normal from normal map in range [0, 1]
    vec3 normal = texture(texture_normal1, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1, 1]
    normal = normal * 2.0 - 1.0;
    normal = normalize(fs_in.TBN * normal);

    // Get diffuse color
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    // Ambient
    vec3 ambient = 0.5 * color;
    vec3 lightDir = normalize(fs_in.LightPos  - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // Specular
    vec3 viewDir = normalize(fs_in.ViewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = color * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}