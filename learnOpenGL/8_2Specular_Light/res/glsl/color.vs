#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalTransMat;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);  // 摄像 view 视角坐标
    FragPos = vec3(model * vec4(aPos, 1.0f));   // 世界坐标 光照不受 摄像机 view 视角的影响
    //Normal = mat3(transpose(inverse(model))) * aNormal;     // 世界坐标下的法线向量  这一步最好在 CPU 上面进行计算
    Normal = mat3(normalTransMat) * aNormal;    // 在 CPU 上计算法线矩阵
}