#include <GL/glew.h>    // 应该放在其他的头文件的前面
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>  // must be included for using stringstream

#ifdef __clang__
// clang 和 clang++
    #define ASSERT(x) if(!(x)) __builtin_debugtrap();
#endif /* __clang__ */

#ifdef __GCC__
// gcc 和 g++
    #define ASSERT(x) if(!(x)) __builtin_trap();
#endif /* __GCC__ */

#ifdef __MSVC__
// windows MSVC cl 编译器
    #define ASSERT(x) if (!(x)) __debugbreak();
#endif

#define GLCALL(x) GLClearError(); \
    x; \
    ASSERT(GLCheckError(#x, __FILE__, __LINE__))


static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool GLCheckError(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error](" << error << "): " << function
            << " " << file << ": " << line << std::endl;
        return false;
    }
    return true;
}


struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};


// 从文件当中解析两个 shader 
static ShaderProgramSource ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

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

    return { ss[static_cast<int>(ShaderType::VERTEX)].str(), ss[static_cast<int>(ShaderType::FRAGMENT)].str() };
}

static unsigned int CompileShader(const std::string& source, unsigned int type) {
    unsigned int id = glCreateShader(type);
    // 你需要保证这个 source 指向的字符串的内存一直处在有效的状态
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // TODO: 错误处理
    // 我们可以检查 glCompileShader 编译生成着色器的结果，因为这个函数不会返回任何的值。从这个函数
    // 本身，我们看不出任何和编译结果相关的状态。
    int result = {0};
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        // 如果我们出现了错误，我们首先就会想知道错误信息的长度
        int length = {0};
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        // 我们知道 length 是一个动态的数字，一般情况下我们都需要在堆上创建 message 了
        // alloca 允许我们在栈上创建一个动态的数组 。期待这个数组不是一个很长的数组
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        // 现在我们获取到了错误信息，可以选择将其打印出来
        std::cout << "============= Create Shader Error =============" << std::endl;
        // 错误信息自动带上了换行符
        std::cout << message ;
        std::cout << "===============================================" << std::endl;

        // 因为创建这个着色器失败了，删除编译这个着色器创建的临时文件
        glDeleteShader(id);
        return 0;
    }

    return id;
}

/**
 * @brief 创建顶点着色器和像素着色器
 * 
 * @param vertexShader 提供给 OpenGL 的顶点着色器的源代码
 * @param fragmentShader 提供给 OpenGL 的像素着色器的源代码
 * @return int OpenGL 返回给我们的着色器的索引值，可以在程序当中使用它
 */
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    // glCreateProgram 这个函数和其他的 OpenGL 的函数的设计方式不同，它不需要传递给他一个 index 索引值。但是它会
    // 返回一个整型的数据
    unsigned int program  = glCreateProgram();
    // 创建我们的两个着色器
    unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

    // 我们需要将我们的两个着色器附加到 OpenGL 的程序当中
    glAttachShader(program, vs);
    glAttachShader(program, fs);

    // 类似于 C++ 代码的编译链接的过程
    glLinkProgram(program);
    glValidateProgram(program);

    // 我们的 shader 已经添加到 OpenGL 的程序当中。所以现在我们可以删除着色器。它就像是在程序编译当中的临时文件，
    // 在链接完完整的程序之后，这些静态的中间文件就可以删除了
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

// ================================================================


int main(int argc, char* argv[]) {
    GLFWwindow* window;
    if (!glfwInit()) {
        return -1;
    }

    // 指定 OpenGL 的接口版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 核心模式，必须自己定义顶点数组对象 VAO
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        std::cout << "glewInit() error!" << std::endl;
    }
    else {
        std::cout << "glewInit() success!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;


    float vertices[] = {
        // First triangle
        -0.9f, -0.5f, 0.0f,  // Left 
        -0.0f, -0.5f, 0.0f,  // Right
        -0.45f, 0.5f, 0.0f,  // Top 
        // Second triangle
        0.0f, -0.5f, 0.0f,  // Left
        0.9f, -0.5f, 0.0f,  // Right
        0.45f, 0.5f, 0.0f   // Top 
    };

    unsigned int vbo, vao;
    GLCALL( glGenVertexArrays(1, &vao) );
    GLCALL( glGenBuffers(1, &vbo) );
    GLCALL( glBindVertexArray(vao) );

    GLCALL( glBindBuffer(GL_ARRAY_BUFFER, vbo) );
    GLCALL( glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW) );
    GLCALL( glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<const void*>(0)) );
    GLCALL( glEnableVertexAttribArray(0) );

    // 解绑
    GLCALL( glBindBuffer(GL_ARRAY_BUFFER, 0) );
    GLCALL( glBindVertexArray(0) );

    // shader
    ShaderProgramSource shader_sources = ParseShader("./res/shader.shader");
    GLCALL( unsigned int shader = CreateShader(shader_sources.VertexSource, shader_sources.FragmentSource) );
    GLCALL( glUseProgram(shader) );



    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        GLCALL( glBindVertexArray(vao) );
        GLCALL( glDrawArrays(GL_TRIANGLES, 0, 6) );
        GLCALL( glBindVertexArray(0) );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    GLCALL( glDeleteVertexArrays(1, &vao) );
    GLCALL( glDeleteBuffers(1, &vbo) );
    glfwTerminate();
    return 0;
}