# 平行光

当一个光源处于很远的地方时，来自光源的每条光线就会近似于互相平行。不论物体和/或者观察者的位置，看起来好像所有的光都来自于同一个方向。当我们使用一个假设光源处于**无限**远处的模型时，它就被称为定向光，因为它的所有光线都有着相同的方向，它与光源的位置是没有关系的。

因为所有的光线都是平行的，所以物体与光源的相对位置是不重要的，因为对场景中每一个物体光的方向都是一致的。由于光的位置向量保持一致，场景中每个物体的光照计算将会是类似的。

我们可以定义一个光线方向向量而不是位置向量来模拟一个定向光。着色器的计算基本保持不变，但这次我们将直接使用光的```direction```向量而不是通过```position```来计算```lightDir```向量。

```glsl
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
```

