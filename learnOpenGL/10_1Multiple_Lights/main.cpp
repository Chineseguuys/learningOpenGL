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
#include "attenuation.h" // 点光源的衰减

// function defination
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void togger_button(const char *str_id, bool *v);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

const char *COLOR_VERTEX_SHADER_FILEPATH = "res/glsl/lightingMaps.vs";
const char *COLOR_FRAGMENT_SHADER_FILEPATH = "res/glsl/lightingMaps.fs";
const char *LIGHT_VERTEX_SHADER_FILEPATH = "res/glsl/light_cube.vs";
const char *LIGHT_FRAGMENT_SHADER_FILEPATH = "res/glsl/light_cube.fs";

const char *TEXTURE1_IMAGE_FILEPATH = "res/textures/awesomeface.png";
const char *TEXTURE2_IMAGE_FILEPATH = "res/textures/container.jpg";
const char *TEXTURE3_IMAGE_FILEPATH = "res/textures/container2.png";
const char *SPECULAR1_LIGHTMAP_FILEPATH = "res/textures/container2_specular.png";
const char *SPECULAR2_LIGHTMAP_FILEPATH = "res/textures/lighting_maps_specular_color.png";
const char *EMISSION1_MAP_FILEPATH = "res/textures/matrix.jpg";

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

float shinining = 64.0f;
bool shiningFlip = false;

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

    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    // 定义 10 个箱子的位置， 这 10 个箱子都使用上面的立方体的模型
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

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

    // 加载图片
    unsigned int diffuseMap = loadTexture(TEXTURE3_IMAGE_FILEPATH);
    // 加载镜面光照贴图
    unsigned int specularMap = loadTexture(SPECULAR1_LIGHTMAP_FILEPATH);
    // 自发光贴图
    unsigned int emissionMap = loadTexture(EMISSION1_MAP_FILEPATH);
    // 设置 texture
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    // 不同的材质对镜面光照的反射的情况不同
    // 例如木板对于镜面光照的强度不高，但是金属对于镜面广告的强度要比木板高很多
    lightingShader.setInt("material.specular", 1);
    // 有一些物体的发光不受到其他光照的影响
    // lightingShader.setInt("material.emission", 2);

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
            ImGui::SliderFloat("shininess", &shinining, 2.0f, 128.0f);
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

        // 绑定 漫反射 map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        // 绑定 镜面反射贴图
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

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
        lightingShader.setFloat("light.constant", attenuation_100.constants);  // 衰减的线性常熟
        lightingShader.setFloat("light.linear", attenuation_100.linear);       // 一次常数
        lightingShader.setFloat("light.quadratic", attenuation_100.quadratic); // 二次常数

        // 镜面反光的属性值，这个值越大，反射光越强越越集中
        lightingShader.setFloat("material.shininess", shinining);

        // 设置手电筒相关
        lightingShader.setVec3("light.position", camera.mPosition);
        lightingShader.setVec3("light.direction", camera.mFront);
        lightingShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f))); // 这里送入的是 \cos(\Phi) 而不直接送入角度
        lightingShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
        lightingShader.setVec3("viewPos", camera.mPosition);

        // 世界矩阵 + view 矩阵 + 投影矩阵
        glm::mat4 projection = glm::perspective(glm::radians(camera.mZoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMatrix4f("projection", projection);
        lightingShader.setMatrix4f("view", view);
        lightingShader.setVec3("viewPos", camera.mPosition);

        // world transformation
        glBindVertexArray(cubeVAO); // 只需要绑定一次即可
        glm::mat4 model = glm::mat4(1.0f);
        for (int idx = 0; idx < sizeof(cubePositions) / sizeof(glm::vec3); ++idx)
        {
            // 在 CPU 上计算法线矩阵
            float rotateAngle = 20.0f * idx;
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[idx]);
            model = glm::rotate(model, glm::radians(rotateAngle), glm::vec3(1.0f, 3.0f, 0.5f));

            glm::mat4 normalModel = glm::transpose(glm::inverse(model));
            lightingShader.setMatrix4f("normalTransMat", normalModel);
            lightingShader.setMatrix4f("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

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

unsigned int loadTexture(const char *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponent;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponent, 0);

    if (data)
    {
        GLenum format;
        if (nrComponent == 1)
        {
            format = GL_RED;
        }
        else if (nrComponent == 3)
        {
            format = GL_RGB;
        }
        else if (nrComponent == 4)
        {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void togger_button(const char *str_id, bool *v)
{
    ImVec4 *colors = ImGui::GetStyle().Colors;
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 1.55f;
    float radius = height * 0.50f;

    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked())
        *v = !*v;
    ImGuiContext &gg = *GImGui;
    float ANIM_SPEED = 0.085f;
    if (gg.LastActiveId == gg.CurrentWindow->GetID(str_id)) // && g.LastActiveIdTimer < ANIM_SPEED)
        float t_anim = ImSaturate(gg.LastActiveIdTimer / ANIM_SPEED);
    if (ImGui::IsItemHovered())
        draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? colors[ImGuiCol_ButtonActive] : ImVec4(0.78f, 0.78f, 0.78f, 1.0f)), height * 0.5f);
    else
        draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? colors[ImGuiCol_Button] : ImVec4(0.85f, 0.85f, 0.85f, 1.0f)), height * 0.50f);
    draw_list->AddCircleFilled(ImVec2(p.x + radius + (*v ? 1 : 0) * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}