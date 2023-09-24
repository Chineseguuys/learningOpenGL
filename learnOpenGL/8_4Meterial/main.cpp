#define __USING_IMGUI__
#define __GL_DEEP_TEST__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <math.h>
#include "Shader.h"
#include "stbi_images.h"

#ifdef __USING_IMGUI__
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Material.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

const char *COLOR_VERTEX_SHADER_FILEPATH = "res/glsl/material.vs";
const char *COLOR_FRAGMENT_SHADER_FILEPATH = "res/glsl/material.fs";
const char *LIGHT_VERTEX_SHADER_FILEPATH = "res/glsl/light_cube.vs";
const char *LIGHT_FRAGMENT_SHADER_FILEPATH = "res/glsl/light_cube.fs";

const char *TEXTURE1_IMAGE_FILEPATH = "res/textures/awesomeface.png";
const char *TEXTURE2_IMAGE_FILEPATH = "res/textures/container.jpg";

struct camera_view
{
    float camera_radius;
    float camera_angle;
};

float deltaTime = 0.0f;
float lastRenderTime = 0.0f;
float cameraSpeedFactor = 2.5f;

// 鼠标相关设置
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool fristMouseMove = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;

// Imgui
int materialSelected = 1;

// Camera
glm::vec3 lightPos(1.2f, 1.0f, 2.0f); // 光源在世界坐标中的位置
float ambientStrength = 0.1f;

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
    GLFWwindow *mainWindow;
    GLFWwindow *imguiWindow;

    mainWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (mainWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    imguiWindow = glfwCreateWindow(SCR_WIDTH / 2, SCR_HEIGHT / 2, "Setting", NULL, NULL);
    if (imguiWindow == NULL)
    {
        std::cout << "Failed to create GLFW Setting" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(mainWindow);
    glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(mainWindow, mouse_callback);
    glfwSetScrollCallback(mainWindow, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

#ifdef __GL_DEEP_TEST__
    glEnable(GL_DEPTH_TEST);
#endif

    Shader lightingShader(COLOR_VERTEX_SHADER_FILEPATH, COLOR_FRAGMENT_SHADER_FILEPATH);
    Shader lightCubeShader(LIGHT_VERTEX_SHADER_FILEPATH, LIGHT_FRAGMENT_SHADER_FILEPATH);

    // 给出了一个立方体的所有的顶点 每一个面具有两个三角形，一个有六个顶点，顶点当中有重复
    float vertices[] = {
        // position            // texture coord  // 法线向量
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, // 4
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,  // 9
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,   // 14
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,   // 19
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,  // 24
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, // 29

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 34
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // 39
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,   // 44
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,   // 49
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // 54
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 59

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,   // 64
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,  // 69
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, // 74
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, // 79
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // 84
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,   // 89

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // 94
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  // 99
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 104
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 109
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // 114
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // 119

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, // 124
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,  // 129
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,   // 134
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,   // 139
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,  // 144
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, // 149

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};

    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // ImGui 的初始化过程
#ifdef __USING_IMGUI__
    // ImGui
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGlfw_InitForOpenGL(imguiWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");
#endif

    // 光照的颜色
    glm::vec3 lightColor;
    lightColor.x = 1.0f;
    lightColor.y = 1.0f;
    lightColor.z = 1.0f;

    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while (!glfwWindowShouldClose(mainWindow) && !glfwWindowShouldClose(imguiWindow))
    {
        glfwMakeContextCurrent(imguiWindow);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
#ifdef __GL_DEEP_TEST__
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else  /* __GL_DEEP_TEST__ */
        glClear(GL_COLOR_BUFFER_BIT);
#endif /* __GL_DEEP_TEST__ */

#ifdef __USING_IMGUI__
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("LIGHTING SETTING");
            ImGui::ColorEdit3("cube color", &lightColor[0]);
            ImGui::RadioButton("emerald", &materialSelected, 0);
            ImGui::RadioButton("jade", &materialSelected, 1);
            ImGui::RadioButton("obsidian", &materialSelected, 2);
            ImGui::RadioButton("pearl", &materialSelected, 3);
            ImGui::RadioButton("rupy", &materialSelected, 4);
            ImGui::RadioButton("gold", &materialSelected, 5);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(imguiWindow);
#endif /* __USING_IMGUI__ */

        glfwMakeContextCurrent(mainWindow);
        processInput(mainWindow);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
#ifdef __GL_DEEP_TEST__
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
        glClear(GL_COLOR_BUFFER_BIT);
#endif

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        lightingShader.use();
        // 光照射到物体上的环境光照应该比较的暗，否则会影响其他的两种光照
        lightingShader.setVec3("light.ambient", ambientColor);
        // 漫反射的光照比环境光照强一些，但是也比较的弱
        lightingShader.setVec3("light.diffuse", diffuseColor);
        // 镜面光照我们希望最强，所以设置为 1.0f, 已最大的程度来反射光线
        lightingShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setVec3("light.position", lightPos);

        switch (materialSelected)
        {
        case 0:
            // material 的属性
            lightingShader.setVec3("material.ambient", emerald.ambient);
            lightingShader.setVec3("material.diffuse", emerald.diffuse);
            lightingShader.setVec3("material.specular", emerald.specular);
            lightingShader.setFloat("material.shininess", emerald.shininess);
            break;
        case 1:
            // material 的属性
            lightingShader.setVec3("material.ambient", jade.ambient);
            lightingShader.setVec3("material.diffuse", jade.diffuse);
            lightingShader.setVec3("material.specular", jade.specular);
            lightingShader.setFloat("material.shininess", jade.shininess);
            break;
        case 2:
            // material 的属性
            lightingShader.setVec3("material.ambient", obsidian.ambient);
            lightingShader.setVec3("material.diffuse", obsidian.diffuse);
            lightingShader.setVec3("material.specular", obsidian.specular);
            lightingShader.setFloat("material.shininess", obsidian.shininess);
            break;
        case 3:
            // material 的属性
            lightingShader.setVec3("material.ambient", pearl.ambient);
            lightingShader.setVec3("material.diffuse", pearl.diffuse);
            lightingShader.setVec3("material.specular", pearl.specular);
            lightingShader.setFloat("material.shininess", pearl.shininess);
            break;
        case 4:
            // material 的属性
            lightingShader.setVec3("material.ambient", rupy.ambient);
            lightingShader.setVec3("material.diffuse", rupy.diffuse);
            lightingShader.setVec3("material.specular", rupy.specular);
            lightingShader.setFloat("material.shininess", rupy.shininess);
            break;
        case 5:
            // material 的属性
            lightingShader.setVec3("material.ambient", gold.ambient);
            lightingShader.setVec3("material.diffuse", gold.diffuse);
            lightingShader.setVec3("material.specular", gold.specular);
            lightingShader.setFloat("material.shininess", gold.shininess);
            break;
        default:
            // material 的属性
            lightingShader.setVec3("material.ambient", emerald.ambient);
            lightingShader.setVec3("material.diffuse", emerald.diffuse);
            lightingShader.setVec3("material.specular", emerald.specular);
            lightingShader.setFloat("material.shininess", emerald.shininess);
        }

        // 世界矩阵 + view 矩阵 + 投影矩阵
        glm::mat4 projection = glm::perspective(glm::radians(camera.mZoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMatrix4f("projection", projection);
        lightingShader.setMatrix4f("view", view);
        lightingShader.setVec3("viewPos", camera.mPosition);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        // 在 CPU 上计算法线矩阵
        glm::mat4 normalModel = glm::transpose(glm::inverse(model));
        lightingShader.setMatrix4f("normalTransMat", normalModel);
        lightingShader.setMatrix4f("model", model);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lightCubeShader.use();
        lightCubeShader.setMatrix4f("projection", projection);
        lightCubeShader.setMatrix4f("view", view);
        lightCubeShader.setVec3("lightCubeColor", lightColor);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMatrix4f("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);

        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
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
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    { // W 拉近物体
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    { // A 向左
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    { // S 拉远物体
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    { // D 拉远物体
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    { // 上键 向上移动物体
        camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    { // 下键  向下移动物体
        camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
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

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    // 窗口失去焦点，不再响应鼠标的事件 该函数在窗口模式下有效
    if (glfwGetWindowAttrib(window, GLFW_FOCUSED) == false)
        return;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (fristMouseMove)
    {
        lastX = xpos;
        lastY = ypos;
        fristMouseMove = false;
    }

    float xoffset = (xpos - lastX);
    float yoffset = (ypos - lastY);
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    // 如果窗口失去了焦点，不再响应鼠标的事件
    if (glfwGetWindowAttrib(window, GLFW_FOCUSED) == false)
        return;
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}