#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>


#define CPP_HIDE __attribute__((visibility(hidden)))

// 局部函数
CPP_HIDE void framebuffer_size_callback(GLFWwindow * window, int width, int height);

// 窗口大小的设定
CPP_HIDE const unsigned int SCR_WIDTH = 800;
CPP_HIDE const unsigned int SCR_HEIGHT = 600;


int main(int argc, char* argv[]) {
    // glfw 的初始化工作
    glfwInit();
    glfwWindowHint(GLFW_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOPENGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glad 用来管理 opengl 的函数指针
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }
}   



void framebuffer_size_callback(GLFWwindow * window, int width, int height) {
    glViewport(0, 0, width, height);
}