#include <GL/glew.h> // 应该放在其他的头文件的前面
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(void)
{
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "glewInit() error!" << std::endl;
    }
    else
    {
        std::cout << "glewInit() success!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    float position[6] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f};

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), position, GL_STATIC_DRAW);

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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<const void *>(0));
    glEnableVertexAttribArray(0);

    // 窗口的大小发生改变的时候，保证三角形的绘制随着画面大小的改变而改变
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

    glfwTerminate();
    return 0;
}