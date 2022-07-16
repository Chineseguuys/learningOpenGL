#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;      // 漫反射贴图
    sampler2D specular;     // 反射光贴图
    sampler2D emission;     // 自发光贴图

    float shininess;
};

struct Light {
    vec3 position;  // 光源的位置
    vec3 direction; // 光源照射的中心方向
    float cutOff;   // 指定了聚光半径的切光角  落在这个角度之外的物体都不会被这个聚光所照亮 注意这里存储的是 cos 余弦值
    float outerCutOff;  // 我们希望在 cutOff 到 outerCutOff 之间的光线可以进行逐渐的进行衰减，看起来可以更加的真实

    vec3 ambient;   // 指定环境光照的强度，以及环境光照的颜色
    vec3 diffuse;   // 指定漫反射光照的强度，以及漫反射光照的颜色
    vec3 specular;  // 指定镜面光照的强度，一般这个强度值设置为 glm::vec3(1.0f, 1.0f, 1.0f)

    float constant;     // 常数项
    float linear;       // 一次线性项
    float quadratic;    // 二次项
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // 全局的环境光照
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    // 物体的漫反射光照
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max( dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // 镜面反射光照
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow( max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    // 聚光的 smooth 效果
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = (light.cutOff - light.outerCutOff);
    // 注意，在 innerCutOff 内的光线不进行衰减，在 innerCutOff 到 outerCutOff 之间的光线进行衰减，在大于
    // outerCutOff 的部分没有光照。
    float intensity = clamp( (theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse     *= intensity;
    specular    *= intensity;

    // 再次加上距离的衰减
    float distance      = length(light.position - FragPos);
    float attenuation   = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient     *= attenuation;
    diffuse     *= attenuation;
    specular    *= attenuation;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}