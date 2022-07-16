#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader
{
public:
    // 程序ID
    unsigned int ID;

    // 构造器读取并构建着色器
    Shader(const char* vertexPath, const char* fragmentPath);
    // 使用/激活程序
    void use();
    // uniform工具函数
    void setBool(const std::string &name, bool value);
    void setInt(const std::string &name, int value);
    void setFloat(const std::string &name, float value);
    void setVec2(const std::string &name, const glm::vec2& value);
    void setVec2(const std::string &name, float x, float y);
    void setVec3(const std::string &name, const glm::vec3& value);
    void setVec3(const std::string &name, float x, float y, float z);
    void setVec4(const std::string &name, const glm::vec4& value);
    void setVec4(const std::string &name, float x, float y, float z, float w);
    void setMatrix4f(const std::string &name, const glm::mat4& matrix);

private:
    void checkCompileErrors(unsigned int shader, std::string type);
    int getLocalUniform(const std::string& name);

private:
    // uniform cache
    std::unordered_map<std::string, int> m_UniformLocationCache;
};


#endif