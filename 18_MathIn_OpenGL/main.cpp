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
#include "Texture.h"

// 已经提前安装在系统 include 目录当中，这里不再单独创建子工程文件夹
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#ifdef DEBUG
#include <glm/gtx/string_cast.hpp>
#endif /* DEBUG */

// 有关矩阵和变换的介绍的内容可以看这里：https://learnopengl.com/Getting-started/Transformations

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


    // 顶点坐标和纹理坐标
    // 顶点的坐标以中间为坐标的零点(0,0)
    // 纹理坐标则从图片的左下角开始为 (0,0)；右上角为(1, 1) 两个坐标的标准不同
    // 实际的坐标应该是 4 维的向量，对于平面图像，可以只指定前面的两个维度的数据 这里补充了后面的两个维度
    float position1[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  -0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f,   0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f
    };

    float position[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f,  -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        1.0f,   1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2 ,   // 这些顶点构成了第一个三角形 这个三角形位于右下角 两个三角形共用了两个顶点
        2, 3, 0     // 这些顶点构成了第二个三角形 这个三角形位于左上角
    };

    // 涉及到混合。注意到图片 ChernoLogoAlpha.png 是一个包含了 Alpha 透明度分量的图片，
    // 在涉及到透明分量的渲染的时候，就需要确定透明部分的叠加和渲染方式。默认的渲染方式往往
    // 无法满足我们的需求
    // 另外的两个 jpg 的图片没有透明的分量，因此渲染的时候不存在 png 存在的问题
    GLCALL( glEnable(GL_BLEND) );
    // blendFunc 指定了混合的方式， src 是我们需要渲染的素材的 RGBA 数据。dst 则是我们要渲染到的缓冲区（在这里可以理解为窗口的背景）
    // 我们需要将我们的图片渲染到背景当中，如果图片是半透明或者全透明的，那么就需要考虑前景色和背景色混合之后的颜色。(就像将一个红色的玻
    // 璃和一个绿色的玻璃叠在一起, 叠在一起的颜色)
    // 针对每一个像素点，下面的代码的算法公式为：
    // R = R_src * A_src + (1 - A_src) * R_dst
    // G = G_src * A_src + (1 - A_src) * G_dst
    // B = B_src * A_src + (1 - A_src) * B_dst
    // A = A_src * A_src + (1 - A_src) * A_dst
    // 混合之后的颜色根据上面的公式来进行计算
    GLCALL( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
    //GLCALL( glBlendFunc(GL_ONE, GL_ZERO) );   // 这是 OpenGL 默认的混合的方式
    GLCALL( glBlendEquation(GL_FUNC_ADD) );
    // 创建顶点数组对象
    //unsigned int vao;
    //GLCALL( glGenVertexArrays(1, &vao) );
    //GLCALL( glBindVertexArray(vao) );

    // 每一个顶点具有四个浮点数，其中的两个是顶点坐标，另外的两个是纹理坐标
    VertexBuffer* vb =  new VertexBuffer(position , 4 * 6 * sizeof(float) );
    VertexArray va;
    VertexBufferLayout layout;
    // 前面的两个是顶点坐标，另外两个是纹理坐标
    layout.Push<float>(4);
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


    // 这里我们需要创建一个正交投影矩阵
    glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    //glm::mat4 proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);

#ifdef DEBUG
    printf("[%s:L%d]mat proj = %s\n", __FUNCTION__, __LINE__, glm::to_string(proj).c_str());
#endif /* DEBUG */

    Shader shader("./res/shaders/Basic.shader");
    shader.Bind();


    // 纹理
    Texture texture("./res/textures/Vulcan-1-3.png");
    // 我们将纹理绑定到 0 号插槽当中
    texture.Bind(0);
    // 我们的纹理被绑定到了 0 插槽当中，所以下面的参数传递为 0
    shader.SetUniform1i("u_Texture", 0);
    // 这个 uniform variable 目前已经不需要再使用了。对于在 shader 当中没有被使用的
    // Uniform variable， 在 C 程序当中无法再找到这个变量，并且无法设置值
    //shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

    // 设置转换矩阵
    shader.SetUniformMat4f("u_MVP", proj);

    GLCALL( glBindVertexArray(0) );


    shader.UnBind();
    va.UnBind();
    vb->Unbind();
    ib->Unbind();

    Renderer renderer;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCALL( glClearColor(0.0f, 0.0f, 1.0f, 1.0f) ); // background color
        GLCALL( glClear(GL_COLOR_BUFFER_BIT) );
        renderer.Clear();
/**
        // 添加代码在这个黑色的窗口里面渲染一个三角形
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f,   0.0f);
        glVertex2f(0.5f,  -0.5f);
        glEnd();
*/


        shader.Bind();
        
        // Draw() 函数当中包含了 VertexArray 以及 IndexBuffer 和 Shader 的绑定 Bind() 操作
        renderer.Draw(va, *ib, shader);

        // GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

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