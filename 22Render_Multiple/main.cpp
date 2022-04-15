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

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// 有关矩阵和变换的介绍的内容可以看这里：https://learnopengl.com/Getting-started/Transformations
// 这里使用的 ImGui 版本和原始视频中的版本在用法上有一定的差异，因此代码和视频中并不是完全对应的。但是结构保持一致
// 如视频中所示，使用 ImGui 可以方便的提供一种实时修改 OpenGL 相关参数的方式，可以更加直观的观察 OpenGL 相关设置
// 对渲染带来的影响

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
/*
    float position1[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  -0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f,   0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f
    };

    float position2[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f,  -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        1.0f,   1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f
    };
*/

    // 由于投影矩阵的存在，我们顶点数组的坐标的中心点从绘制窗口的中心移动到了绘制窗口的左下角
    // 可以渲染的顶点的坐标范围不再是 (-1.0 - 1.0)。而扩展到了渲染窗口的分辨率大小
    // 这里渲染出来一个边长为 100 像素的正方形
    float position[] = {
        -50.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         50.0f, -50.0f, 0.0f, 1.0f, 1.0f, 0.0f,
         50.0f,  50.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -50.0f,  50.0f, 0.0f, 1.0f, 0.0f, 1.0f
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

    IndexBuffer* ib = new IndexBuffer(indices, 6);


    // 这里我们需要创建一个正交投影矩阵.
    /**
     * -1.0f 1.0f 为 x 轴的范围，意思也就是说，范围在 -1.0 到 1.0 范围内的顶点会被渲染在窗口当中
     * -0.75f 0.75f 意义也是相同
     * 需要注意 position 中的顶点坐标的坐标值的范围。 比如， 第一个顶点的位置为 -0.5 0.5 。那么也就意味着在 x 轴的方向
     * 这个顶点位于 x 轴左侧的 1/4 长度的位置(屏幕的中央的位置为 0， 0)
     */
    //glm::mat4 proj = glm::ortho(-1.0f, 1.0f, -0.75f, 0.75f, -1.0f, 1.0f);
    //glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    //glm::mat4 proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);

    // 将我们的投影矩阵设置为窗口的分辨率。为了在窗口中看见纹理，需要相应的改变顶点的坐标
    // projection 还是将顶点的坐标映射到 (-1.0f 到 1.0f) 之间
    glm::mat4 proj = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f);
    // 相机变换 观察模型的视角，类似与使用相机在特定的位置和特定的角度对模型进行观察
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
#ifdef DEBUG
    printf("[%s:L%d]mat4(1.0f) is %s\n", __FUNCTION__, __LINE__, glm::to_string(glm::mat4(1.0f)).c_str());
    printf("[%s:L%d]view is %s\n", __FUNCTION__, __LINE__, glm::to_string(view).c_str());
#endif /* DEBUG */

    glm::vec3 translationA(100, 100, 0);
    glm::vec3 translationB(300, 100, 0);

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

    GLCALL( glBindVertexArray(0) );


    shader.UnBind();
    va.UnBind();
    vb->Unbind();
    ib->Unbind();

    Renderer renderer;

    // ImGui
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCALL( glClearColor(0.0f, 0.0f, 1.0f, 1.0f) ); // background color
        GLCALL( glClear(GL_COLOR_BUFFER_BIT) );
        renderer.Clear();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        shader.Bind();

        {
            // 模型变换 表示对当前的需要绘制的对象进行变换
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
            // 注意矩阵乘法的方式从右边向左边
            glm::mat4 mvp = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp);
            // Draw() 函数当中包含了 VertexArray 以及 IndexBuffer 和 Shader 的绑定 Bind() 操作
            renderer.Draw(va, *ib, shader);
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
            glm::mat4 mvp = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp);
            // Draw() 函数当中包含了 VertexArray 以及 IndexBuffer 和 Shader 的绑定 Bind() 操作
            renderer.Draw(va, *ib, shader);
        }

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

            ImGui::SliderFloat3("TranslationA", &translationA.x, 0.0f, 640.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat3("TranslationB", &translationB.x, 0.0f, 640.0f);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}