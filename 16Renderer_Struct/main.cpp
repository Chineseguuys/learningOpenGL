#include <GL/glew.h>    // 应该放在其他的头文件的前面
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>  // must be included for using stringstream

#include "ErrorCheck.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Renderer.h"

// https://learnopengl-cn.readthedocs.io/zh/latest/01%20Getting%20started/04%20Hello%20Triangle/

/**
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
*/

/**
 * @brief 创建顶点着色器和像素着色器
 * 
 * @param vertexShader 提供给 OpenGL 的顶点着色器的源代码
 * @param fragmentShader 提供给 OpenGL 的像素着色器的源代码
 * @return int OpenGL 返回给我们的着色器的索引值，可以在程序当中使用它
 */
/**
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
**/

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // OpenGL的核心模式要求我们使用VAO，所以它知道该如何处理我们的顶点输入。如果我们绑定VAO失败，OpenGL会拒绝绘制任何东西
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "glewInit() error!" << std::endl; 
    } else {
        std::cout << "glewInit() success!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    /**
     * @brief 在这些顶点的数据中，包含了两个三角形的所有的顶点。由于两个三角形的斜边是重合的，意味着
     * 其中的两个顶点的坐标的值是完全一致的。这浪费了大量的内存空间
     */
/*
    float position[] = {
        -0.5f, -0.5f,   // 第一个直角三角形
        0.5f,  -0.5f,
        0.5f,   0.5f,

        0.5f,   0.5f,   // 第二个直角三角形
        -0.5f,  0.5f,
        -0.5f,  -0.5f
    };
*/

    float position[] = {
        -0.5f, -0.5f,   
        0.5f,  -0.5f,
        0.5f,   0.5f,
        -0.5f,  0.5f
    };

    unsigned int indices[] = {
        0, 1, 2 ,   // 这些顶点构成了第一个三角形
        2, 3, 0     // 这些顶点构成了第二个三角形
    };


    // 创建顶点数组对象
    //unsigned int vao;
    //GLCALL( glGenVertexArrays(1, &vao) );
    //GLCALL( glBindVertexArray(vao) );

    VertexBuffer* vb =  new VertexBuffer(position , 4 * 2 * sizeof(float) );
    VertexArray va;
    VertexBufferLayout layout;
    layout.Push<float>(2);
    va.AddBuffer(*vb, layout);

    //unsigned int buffer;
    //glGenBuffers(1, &buffer);
    //glBindBuffer(GL_ARRAY_BUFFER, buffer);
    /**
     * GL_STATIC_DRAW: 数据不会或者几乎不会改变
     * GL_DYNAMIC_DRAW: 数据会被改变很多次     
     * GL_STREAM_DRAW: 数据在每一次绘制的时候都会改变
     */
    //glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), position, GL_STATIC_DRAW);

    /**
     * 第一个参数 0 表示属性的索引。我们的 buffer 当中存储的每一个顶点可能具有多个属性 {位置，纹理坐标，法线方向}。它们的存储的方式可能是下面这样的
     * {{位置1，纹理坐标1，法线方向1}, {位置2，纹理坐标2，法线方向2}, {...}, ...} 
     * 我们这里的 position 只用来表示二维的位置。没有其他的属性信息。所以只需要调用一次 glVertexAttribPointer() 函数。记住在顶点着色器当中，我们定义
     * 了类似于下面的代码：
     *          layout(location = 0) in vec4 position;
     * 上面的这一行代码定义了 position 顶点属性的位置值。它可以把顶点属性的位置值设置为0。因为我们希望可以把数据传递到这一个顶点属性当中去，所以第一个参数需要
     * 传入 0
     * 
     * 第二个参数 2 表示的是每一个位置属性所包含的数据量，因为我们的每一个位置数据包含了两个浮点数值，所以这里的值是 2
     * shaders_works
     * GL_FLOAT 用来说明位置数据的值是浮点数值
     * 
     * GL_FALSE 表示不需要 opengl 来为我们做 normalized
     * 
     * 2 * sizeof(flaot) 实际上用来说明两个顶点数据之间的距离。比方说，我们的顶点具有三个属性 {位置，纹理坐标，法线方向}。位置使用三个浮点数来进行表示(12个字节)，
     * 纹理坐标使用两个浮点数(8个字节)来进行表示；法线方向使用三个浮点数(12 个字节)来进行表示；那么每一个顶点的数据需要使用 32 个字节的数据来进行表示。
     * 那么在连续存储的大量的顶点数据当中，第一个顶点数据开始的位置和第二个顶点数据的开始的位置的偏移值是 32 个字节 。 在下面的代码当中，每个位置使用两个浮点数来进行
     * 表示，那么两个顶点之间的偏移量就是 8 
     * 
     * 最后一个参数 0  表示的是每一个属性值在顶点内部的偏移位置。比方说，位置是顶点的第一个属性，所以它的偏移位置为0。纹理坐标是第二个属性，它的偏移位置是 12， 
     * 法线方向是第三个属性，他的偏移位置是 20。 简单的理解为结构体内部的偏移量就可以了
     * 
     */
    //GLCALL( glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<const void*>(0)) );
    //GLCALL( glEnableVertexAttribArray(0) );


    /**
     * @brief 经过了上面的步骤，我们把顶点数据存储到显卡的内存当中，用 buffer 这个顶点缓冲对象来对其进行管理。
     * 这个缓冲对象的类型是 GL_ARRAY_BUFFER 类型。
     */

    IndexBuffer* ib = new IndexBuffer(indices, 6);
    //unsigned int ibo;
    //glGenBuffers(1, &ibo);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);


    Shader shader("./res/shaders/Basic.shader");
    shader.Bind();

    shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
    //ShaderProgramSource source = ParseShader("./res/shaders/Basic.shader");
    //std::cout << "FragmenetSource" << std::endl;
    //std::cout << source.FragmentSource << std::endl;
    //std::cout << "VertexSource" << std::endl;
    //std::cout << source.VertexSource << std::endl;

    // 如果我们打算做渲染的话，我们至少需要一个顶点着色器和一个片段着色器来处理所有的数据
    //GLCALL( unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource) );
    //GLCALL( glUseProgram(shader) );

    //GLCALL(int location = glGetUniformLocation(shader, "u_Color"));
    //ASSERT(location != -1)
    //GLCALL(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));


    GLCALL( glBindVertexArray(0) );

    // 解绑所有的顶点数据
    //GLCALL( glUseProgram(0) );
    //GLCALL( glBindBuffer(GL_ARRAY_BUFFER, 0) );
    //GLCALL( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
    
    shader.UnBind();
    va.UnBind();
    vb->Unbind();
    ib->Unbind();

    Renderer renderer;

    float r = 0.2f;
    float increment = 0.025;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        //glClear(GL_COLOR_BUFFER_BIT);
        renderer.Clear();
/**
        // 添加代码在这个黑色的窗口里面渲染一个三角形
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f,   0.0f);
        glVertex2f(0.5f,  -0.5f);
        glEnd();
*/

        // 线框模式 只渲染线
        //GLCALL( glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) );
        //GLCALL( glUseProgram(shader) );
        //GLCALL( glUniform4f(location, r, 0.3f, 0.8f, 1.0f) );

        shader.Bind();
        shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

        //GLCALL( glBindBuffer(GL_ARRAY_BUFFER, buffer) );
        //GLCALL( glEnableVertexAttribArray(0) );
        //GLCALL( glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<const void*>(0)) );

        //GLCALL( glBindVertexArray(vao) );
        //va.Bind();

        //ib->Bind();
        //GLCALL( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo) );
        
        // Draw() 函数当中包含了 VertexArray 以及 IndexBuffer 和 Shader 的绑定 Bind() 操作
        renderer.Draw(va, *ib, shader);

        // GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if (r > 1.0f)
            increment = -0.025;
        if (r < 0.0f)
            increment = 0.025;
        
        r += increment;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    /**
     * 这一点你应该主要，在调用 glfwTerminate 之前，你最好自己先析构掉你构建的 IndexBuffer 以及 VertexBuffer
     * 原因在于如果 IndexBuffer 和 VertexBuffer 的析构放在了 glfwTerminate 之后，那么，IndexBuffer 和 
     * VertexBuffer 的析构中的 glDeleteBuffers() 函数将会报错。
     * glfwTerminate 会破坏 OpenGL 的上下文。
     */
    delete vb;
    delete ib;

    glfwTerminate();
    return 0;
}