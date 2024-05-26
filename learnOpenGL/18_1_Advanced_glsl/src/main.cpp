#define __USING_IMGUI__
#define __GL_DEEP_TEST__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include "shader.h"
#include "stbi_images.h"

#ifdef __USING_IMGUI__
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "spdlog/spdlog.h"

#include "camera.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

const char *VERTEX_SHADER_FILEPATH = "../res/glsl/shader.vs";
const char *FRAGMENT_SHADER_FILEPATH = "../res/glsl/shader.fs";


// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouseMove = true;
float yaw = -90.0f;
float lastX = SCR_WIDTH / 2.f;
float lastY = SCR_HEIGHT / 2.f;

float deltaTime = 0.0f;
float lastRenderTime = 0.0f;
float cameraSpeedFactory = 2.5f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 核心模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == nullptr)
    {
        spdlog::critical("Failed to Create GLFW Window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        spdlog::critical("Failed to Initialize GLAD");
        return -1;
    }

#ifdef __GL_DEEP_TEST__
    glEnable(GL_DEPTH_TEST);
#endif

#ifdef ENABLE_POINT_SIZE
    // enable change point size
    glEnable(GL_PROGRAM_POINT_SIZE);
#endif

    Shader shader(VERTEX_SHADER_FILEPATH, FRAGMENT_SHADER_FILEPATH);

    // 给出了一个立方体的所有的顶点 每一个面具有两个三角形，一个有六个顶点，顶点当中有重复
    float vertices[] = {
        // position            // texture coord
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // 4
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  // 9
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // 14
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // 19
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,  // 24
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // 29

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // 34
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,  // 39
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // 44
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // 49
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,  // 54
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // 59

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // 64
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // 69
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // 74
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // 79
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // 84
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // 89

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // 94
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // 99
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // 104
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // 109
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // 114
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // 119

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // 124
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,  // 129
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // 134
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // 139
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // 144
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // 149

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    // 创建变换矩阵 单位矩阵
    const glm::mat4 base = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0f, 0.0f));

    // 透视投影
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                            static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                                            0.1f, 100.0f);

    // 围绕 x 轴进行旋转
    glm::mat4 model = glm::rotate(base, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // lookat 矩阵：LookAt矩阵作为观察矩阵可以很高效地把所有世界坐标变换到刚刚定义的观察空间
    glm::mat4 view = camera.GetViewMatrix();

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // 顶点坐标和纹理坐标
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0); // for location 0
    glEnableVertexAttribArray(1); // for location 1

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

#ifdef __GL_DEEP_TEST__
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
        glClear(GL_COLOR_BUFFER_BIT);
#endif

        // recalcute projection and view matrix
        projection = glm::perspective(glm::radians(camera.mZoom),
                                      static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                                      0.1f,
                                      100.f);
        view = camera.GetViewMatrix();

        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    float currentRenderTime = glfwGetTime();
    deltaTime = currentRenderTime - lastRenderTime;
    lastRenderTime = currentRenderTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouseMove)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouseMove = false;
    }

    float xoffset = (xpos - lastX);
    float yoffset = (ypos - lastY);
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}