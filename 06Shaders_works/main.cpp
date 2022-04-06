#include <GL/glew.h>    // 应该放在其他的头文件的前面
#include <GLFW/glfw3.h>
#include <iostream>

// 着色器的程序代码，使用字符串类型进行保存。当然可以存储在单独的文件当中，然后读取出来
static const std::string SOURCE_VERTEXSHADER = 
    "#version 330 core\n"
    "\n"
    "layout(location = 0) in vec4 position;\n"
    "\n"
    "void main()\n"
    "{\n"
    "     gl_Position = position;\n"
    "}\n";

static const std::string SOURCE_FRAGMENTSHADER = 
    "#version 330 core\n"
    "\n"
    "layout(location = 0) out vec4 color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "     color = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";



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

    glLinkProgram(program);
    glValidateProgram(program);

    // 我们的 shader 已经添加到 OpenGL 的程序当中。所以现在我们可以删除着色器。它就像是在程序编译当中的临时文件，
    // 在链接完完整的程序之后，这些静态的中间文件就可以删除了
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

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

    float position[6] = {
        -0.5f, -0.5f,
        0.0f,   0.5f,
        0.5f,  -0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), position, GL_STATIC_DRAW);

    // 自定义的着色器程序
    unsigned int shader = CreateShader(SOURCE_VERTEXSHADER, SOURCE_FRAGMENTSHADER);
    // 使用这个着色器程序
    glUseProgram(shader);

    /**
     * 第一个参数 0 表示属性的索引。我们的 buffer 当中存储的每一个顶点可能具有多个属性 {位置，纹理坐标，法线方向}。它们的存储的方式可能是下面这样的
     * {{位置1，纹理坐标1，法线方向1}, {位置2，纹理坐标2，法线方向2}, {...}, ...} 
     * 我们这里的 position 只用来表示二位的位置。没有其他的属性信息。所以只需要调用一次 glVertexAttribPointer() 函数
     * 
     * 第二个参数 2 表示的是每一个位置属性所包含的数据量，因为我们的每一个位置数据包含了两个浮点数值，所以这里的值是 2
     * 
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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<const void*>(0));
    glEnableVertexAttribArray(0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
/**
        // 添加代码在这个黑色的窗口里面渲染一个三角形
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f,   0.0f);
        glVertex2f(0.5f,  -0.5f);
        glEnd();
*/

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // 在你使用完这个自定义的着色器程序之后，就可以删除它了
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}