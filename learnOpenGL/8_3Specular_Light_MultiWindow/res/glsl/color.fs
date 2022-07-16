#version 330 core
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform float ambientON;    // 是否打开全局的环境光照
uniform float diffuseON;    // 是否打开漫反射光照
uniform float specularON;   // 是否打开镜面光照
uniform int shininess;    // 反光度
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;

void main()
{
    // 在世界空间当中做光照
    // ambient  // 全局环境光照
    vec3 ambient = ambientON * ambientStrength * lightColor;

    // diffuse  // 漫反射光照
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  // 从片段顶点指向光源的向量
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseON * diff * lightColor;

    // specular // 镜面光照
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);    // 从片段顶点指向人眼的向量
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularON * specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}