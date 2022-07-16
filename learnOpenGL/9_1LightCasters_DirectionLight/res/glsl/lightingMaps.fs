#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;

    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 direction;     // 对于平行光来说，不需要确定光源的位置

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // 在世界空间当中做光照
    // ambient  // 全局环境光照
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(- light.direction);   // 我们认知里面的光线的方向是从光源照射到物体的表面，但是在计算上面，我们需要的是从物体表面指向光源的方向，所以这里添加了 - 号
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * texture(material.diffuse, TexCoords).rgb);

    // 镜面光
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 镜面反射贴图，对不同的材质的镜面反射程度进行设置
    vec3 specular = light.specular * (spec * texture(material.specular, TexCoords).rgb);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}