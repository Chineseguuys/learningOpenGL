#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;      // 漫反射贴图
    sampler2D specular;     // 反射光贴图
    sampler2D emission;     // 自发光贴图

    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;  // 光源的位置
    vec3 direction; // 光源照射的方向
    float cutOff;   // 指定了聚光半径的切光角, 落在这个角度之外的物体都不会被这个聚光所照亮 注意这里存储的是 cos 余弦值
    float outerCutOff;  // // 我们希望在 cutOff 到 outerCutOff 之间的光线可以进行逐渐的进行衰减，看起来可以更加的真实

    float constant; // 衰减函数的常数项
    float linear;   // 衰减函数的一次项
    float quadratic;    // 衰减函数的二次项

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec3 TexCoords;

uniform vec3 viewPos;
uniform DirLight  dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

// 函数声明
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);




void main()
{
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // 计算平行光线和表面的发现方向的点积值
    float diff = max(dot(lightDir, normal), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * vec3(texture(material.specular, TexCoords));

    return (ambient + diffuse + specular);
}