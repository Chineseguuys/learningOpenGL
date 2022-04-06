#ifndef __SHADER_OPENGL__
#define __SHADER_OPENGL__

#include <string>
#include <unordered_map>

struct ShaderProgramSource;

class Shader
{
private:
    // 着色器程序的原始代码文件的路径
    std::string m_FilePath;
    // OpenGL 提供的着色器程序的 ID
    unsigned int m_ShaderID;
    // 我们不希望每一次设置 uniform variable 的时候，都去查询它的 location, 
    // 可以提前把它的 location 进行缓冲。每一次重新设置 uniform variable 的值的时候
    // 都重新根据 name 来查询 location 值，将会浪费很多时间
    std::unordered_map<std::string, int> m_UniformLocationCache;
public:
    Shader(const std::string& filePath);
    ~Shader();

    void Bind() const;
    void UnBind() const;

    // Set uniforms 根据 uniforms 的名字来设置 Uniforms 的值。这里是设置三维的颜色的值，最后一个维度 alpha 
    // 代表了透明度
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float f3);

    void SetUniform1f(const std::string& name, float value);
    void SetUniform1i(const std::string& name, int value);

private:
    ShaderProgramSource ParseShader(const std::string& filePath);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    int GetUniformLocation(const std::string& name);
};


#endif