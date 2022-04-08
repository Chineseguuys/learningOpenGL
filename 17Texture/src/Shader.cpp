#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "ErrorCheck.h"

// 这个类不希望其他的文件可以访问到它
struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};


Shader::Shader(const std::string& filePath)
    : m_FilePath(filePath), 
      m_ShaderID(0) {
    
    // 顶点着色器和片段着色器程序写在同一个文件当中
    ShaderProgramSource source = ParseShader(m_FilePath);
    m_ShaderID = CreateShader(source.VertexSource, source.FragmentSource);
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    GLCALL( unsigned int id = glCreateShader(type) );
    // 你需要保证这个 source 指向的字符串的内存一直处在有效的状态
    const char* src = source.c_str();
    GLCALL( glShaderSource(id, 1, &src, nullptr) );
    GLCALL( glCompileShader(id) );

    // TODO: 错误处理
    // 我们可以检查 glCompileShader 编译生成着色器的结果，因为这个函数不会返回任何的值。从这个函数
    // 本身，我们看不出任何和编译结果相关的状态。
    int result = {0};
    GLCALL( glGetShaderiv(id, GL_COMPILE_STATUS, &result) );
    if (result == GL_FALSE) {
        // 如果我们出现了错误，我们首先就会想知道错误信息的长度
        int length = {0};
        GLCALL( glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length) );
        // 我们知道 length 是一个动态的数字，一般情况下我们都需要在堆上创建 message 了
        // alloca 允许我们在栈上创建一个动态的数组 。期待这个数组不是一个很长的数组
        char* message = (char*)alloca(length * sizeof(char));
        GLCALL( glGetShaderInfoLog(id, length, &length, message) );
        // 现在我们获取到了错误信息，可以选择将其打印出来
        std::cout << "============= Create Shader Error =============" << std::endl;
        // 错误信息自动带上了换行符
        std::cout << message ;
        std::cout << "===============================================" << std::endl;

        // 因为创建这个着色器失败了，删除编译这个着色器创建的临时文件
        GLCALL( glDeleteShader(id) );
        return 0;
    }

    return id;
}

Shader::~Shader() {
    GLCALL( glDeleteProgram(m_ShaderID) );
}

ShaderProgramSource Shader::ParseShader(const std::string& filePath) {
    std::ifstream stream(filePath);

    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while(getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            } 
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[static_cast<int>(type)] << line << "\n";
        }
    }

#ifdef DEBUG
    std::cout << ss[static_cast<int>(ShaderType::VERTEX)].str() << "-----" << std::endl;
    std::cout << ss[static_cast<int>(ShaderType::FRAGMENT)].str() << std::endl;
#endif /*DEBUG*/

    return { ss[static_cast<int>(ShaderType::VERTEX)].str(), ss[static_cast<int>(ShaderType::FRAGMENT)].str() };
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    // glCreateProgram 这个函数和其他的 OpenGL 的函数的设计方式不同，它不需要传递给他一个 index 索引值。但是它会
    // 返回一个整型的数据
    GLCALL( unsigned int program  = glCreateProgram() );
    // 创建我们的两个着色器
    GLCALL( unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader) );
    GLCALL( unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader) );

    // 我们需要将我们的两个着色器附加到 OpenGL 的程序当中
    GLCALL( glAttachShader(program, vs) );
    GLCALL( glAttachShader(program, fs) );

    // 类似于 C++ 代码的编译链接的过程
    GLCALL( glLinkProgram(program) );
    GLCALL( glValidateProgram(program) );

    // 我们的 shader 已经添加到 OpenGL 的程序当中。所以现在我们可以删除着色器。它就像是在程序编译当中的临时文件，
    // 在链接完完整的程序之后，这些静态的中间文件就可以删除了
    GLCALL( glDeleteShader(vs) );
    GLCALL( glDeleteShader(fs) );

    return program;
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float f3) {
    int location = GetUniformLocation(name);
    GLCALL( glUniform4f(location, v0, v1, v2, f3) );
}

void Shader::SetUniform1f(const std::string& name, float value) {
    int location = GetUniformLocation(name);
    GLCALL( glUniform1f(location, value) );
}

void Shader::SetUniform1i(const std::string& name, int value) {
    int location = GetUniformLocation(name);
    GLCALL( glUniform1i(location, value) );
}

int Shader::GetUniformLocation(const std::string& name) {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
        return m_UniformLocationCache[name];
    }

    GLCALL( int location = glGetUniformLocation(m_ShaderID, name.c_str()) );

#ifdef DEBUG
    printf("[%s:L%d]location for %s is %d\n", __FUNCTION__, __LINE__, name.c_str(), location);
#endif /* DEBUG */

    if (location == -1) {
        std::cout << "Warning: uniform " << name << " does not exits!" << std::endl;
    }
    // 我们需要缓冲已经查询过的 uniform variable, 可以提高查询的速度
    m_UniformLocationCache[name] = location;
    return location;
}

void Shader::Bind() const {
    GLCALL( glUseProgram(m_ShaderID) );
}

void Shader::UnBind() const {
    GLCALL( glUseProgram(0) );
}