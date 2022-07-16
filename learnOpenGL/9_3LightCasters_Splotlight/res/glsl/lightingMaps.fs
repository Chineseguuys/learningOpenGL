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
    float outerCutOff;

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
    vec3 lightDir = normalize(light.position - FragPos);

    // 检查光线是否在光源的角度之内
    float theta = dot(lightDir, normalize(-light.direction));

    if (theta > light.cutOff)   
    // 需要注意的是上面的 dot() 函数计算出来的是 theta 角度的 cos 值，所以这里我们使用 > 表示在切光角范围内的物体
    {
        // 全局的环境光照
        vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

        // 物体的漫反射
        vec3 norm       = normalize(Normal);  // 法线方向，垂直于三角形表面的向量
        float diff      = max( dot(norm, lightDir), 0.0);
        vec3 diffuse    = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

        // 物体的镜面反射
        vec3 viewDir    = normalize(viewPos - FragPos);    // 眼睛看向物体表面的方向
        vec3 reflectDir = reflect(-lightDir, norm);     // 光线的反射方向
        float spec      = pow( max( dot(viewDir, reflectDir), 0.0), material.shininess );    // 计算反射光线的强度
        vec3 specular   = light.specular * spec * texture( material.specular, TexCoords).rgb;

        // 光线的衰减
        float distance      = length(light.position - FragPos);
        float attenuation   = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        diffuse     *= attenuation;
        specular    *= attenuation;

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
    else 
    {
        // 对于在光照范围外的物体，我们只给全局的环境光照
        FragColor = vec4(light.ambient * texture(material.diffuse, TexCoords).rgb, 1.0);
    }
}