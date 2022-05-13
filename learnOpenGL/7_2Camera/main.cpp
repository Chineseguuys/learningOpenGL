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
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

const char* VERTEX_SHADER_FILEPATH = "res/glsl/shader.vs";
const char* FRAGMENT_SHADER_FILEPATH = "res/glsl/shader.fs";
const char* TEXTURE1_IMAGE_FILEPATH = "res/textures/awesomeface.png";
const char* TEXTURE2_IMAGE_FILEPATH = "res/textures/container.jpg";

struct camera_view {
    float camera_radius;
    float camera_angle;
};

// main 当中的部分变量移动到外来写成全局变量比较麻烦，所以这里只使用指向它们的指针
glm::vec3* cameraPos_ptr = NULL;
glm::vec3* up_ptr = NULL;
glm::vec3* front_ptr = NULL;


float deltaTime = 0.0f;
float lastRenderTime = 0.0f;
float cameraSpeedFactor = 2.5f;

// 鼠标相关设置
bool fristMouseMove = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;

// Imgui 
float mix_parameter = 0.5f;
float texture_coord = 1.0f;
float fov_angle = 45.0f;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

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

    Shader shader(VERTEX_SHADER_FILEPATH, FRAGMENT_SHADER_FILEPATH);

    // 给出了一个立方体的所有的顶点 每一个面具有两个三角形，一个有六个顶点，顶点当中有重复
    float vertices[] = {
    // position            // texture coord
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   //4
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,   //9
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   //14
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   //19
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   //24
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   //29

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   // 34
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   // 39
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   // 44
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   // 49
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,   // 54
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   // 59

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   // 64
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   // 69
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   // 74
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   // 79
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   // 84
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   // 89

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   // 94
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   // 99
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   // 104
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   // 109
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   // 114
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   // 119

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   // 124
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,   // 129
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   // 134
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   // 139
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   // 144
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   // 149

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    float* texcoord_pointer[] = {
        &vertices[8],
        &vertices[13], &vertices[14],
        &vertices[18], &vertices[19],
        &vertices[24],

        &vertices[38],
        &vertices[43], &vertices[44],
        &vertices[48], &vertices[49],
        &vertices[54],

        &vertices[63],
        &vertices[68], &vertices[69],
        &vertices[74],
        &vertices[79],
        &vertices[88],

        &vertices[93],
        &vertices[98], &vertices[99],
        &vertices[104],
        &vertices[109],
        &vertices[118],

        &vertices[124],
        &vertices[128], &vertices[129],
        &vertices[133],
        &vertices[138],
        &vertices[149],

        &vertices[154],
        &vertices[158], &vertices[159],
        &vertices[163],
        &vertices[168],
        &vertices[179]
    };

    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3 (2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // 后面再尝试精简这些顶点

    // 创建变换矩阵 单位矩阵
    const glm::mat4 base = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0f, 0.0f));

    // 透视投影
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                0.1f, 100.0f);

    // 围绕 x 轴进行旋转
    glm::mat4 model = glm::rotate(base, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // 摄像机位置，使用一个向量来进行表示
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);

    // 摄像机指向的位置，这里设定为本地坐标的原点
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // 摄像机指向向量 这个向量实际上是从原点指向摄像机的向量。和摄像机的指向正好相反
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    // 上向量 指向 y 轴的正方向
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);

    // 我们需要摄像机的右向量，这个向量可以通过上向量和摄像机的指向向量进行叉乘得到
    glm::vec3 cameraRight = glm::normalize( glm::cross( up, cameraDirection) );

    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
    // 现在 cameraDirection 和 cameraRight 以及 cameraUp 三个向量是两两正交的单位向量

    // lookat 矩阵：LookAt矩阵作为观察矩阵可以很高效地把所有世界坐标变换到刚刚定义的观察空间
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, up);

    // 设置全局变量
    cameraPos_ptr = &cameraPos;
    up_ptr = &up;
    front_ptr = &front;

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // 顶点坐标和纹理坐标
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);   // for location 0
    glEnableVertexAttribArray(1);   // for location 1


    // 创建 Texture1
    unsigned int texture1;
    glGenTextures(1, &texture1);
    // 一定要先 bind 之后，设置的属性才可以生效
    glBindTexture(GL_TEXTURE_2D, texture1);
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载一张图片
    int width, height, nrChannels;
    unsigned char* image = stbi_load(TEXTURE1_IMAGE_FILEPATH, &width, &height, &nrChannels, 0);

    if (image != NULL) {
        /**
         * @brief 函数的参数非常的长，下面进行讲解
         * 第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理
         *      （任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）
         * 
         * 第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别
         * 
         * 第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值
         * 
         * 第四个和第五个参数设置最终的纹理的宽度和高度。我们之前加载图像的时候储存了它们，所以我们使用对应的变量。
         * 下个参数应该总是被设为0（历史遗留的问题）
         * 
         * 第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值
         * 
         * 最后一个参数是真正的图像数据
         */

        printf("image size is [%d, %d, %d]\n", width, height, nrChannels);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        /**
         * @brief 这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。因为上面的一个函数相应的值为 0， 所以这里不会自动生成多级渐远的纹理
         */
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

    // 解绑
    glBindTexture(GL_TEXTURE_2D, 0);

    // 创建 Texture
    unsigned int texture2;
    glGenTextures(1, &texture2);
    // 一定要先 bind 之后，设置的属性才可以生效
    glBindTexture(GL_TEXTURE_2D, texture2);
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载一张图片
    image = stbi_load(TEXTURE2_IMAGE_FILEPATH, &width, &height, &nrChannels, 0);

    if (image != NULL) {
        printf("image size is [%d, %d, %d]\n", width, height, nrChannels);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

    // 纹理已经设置，图像可以删除了。
    stbi_image_free(image);


    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    shader.use();
    shader.setInt("Texture1", 0);
    shader.setInt("Texture2", 1);
    shader.setFloat("mixParameter", 0.5);

    shader.setMatrix4f("model", model);
    shader.setMatrix4f("view", view);
    shader.setMatrix4f("projection", projection);

#ifdef __USING_IMGUI__
    // ImGui
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
#endif

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
#ifdef __GL_DEEP_TEST__
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
        glClear(GL_COLOR_BUFFER_BIT)
#endif

#ifdef __USING_IMGUI__
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::Begin("TEST TEXTURE MIX");

            ImGui::Text("change two textures mix parameter");               // Display some text (you can use a format strings too)
            ImGui::SliderFloat("mix param", &mix_parameter, 0.0f, 1.0f);
            ImGui::SliderFloat("texture coord", &texture_coord, 0.0f, 4.0f);
            ImGui::SliderFloat("fov", &fov_angle, 0.0f, 180.0f);
            ImGui::SliderFloat("camera speed", &cameraSpeedFactor, 1.0f, 8.0f);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
#endif /* __USING_IMGUI__ */

        for (float* v : texcoord_pointer) {
            *v = texture_coord;
        }

        projection = glm::perspective(glm::radians(fov_angle), 
                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                0.1f, 100.0f);

        // 左右的水平横移 和前后拉近距离的效果
        view = glm::lookAt(
            cameraPos,
            cameraPos + front,
            up
        );

        // 是否是正确的方式 需要不断的更新
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        // 激活特定的插槽
        // 绑定 texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        shader.use();
        shader.setFloat("mixParameter", mix_parameter);
        shader.setMatrix4f("projection", projection);
        shader.setMatrix4f("view", view);
        glBindVertexArray(VAO);

        for (unsigned int i = 0; i < 10; ++i) {
            model = glm::translate(base, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(0.5f, 0.0f, 0.0f));
            shader.setMatrix4f("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);

#ifdef __USING_IMGUI__
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif /* __USING_IMGUI__ */

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);

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
    float cameraSpeed = deltaTime * cameraSpeedFactor;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        *cameraPos_ptr += cameraSpeed * (*front_ptr);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        *cameraPos_ptr -= glm::normalize(glm::cross(*front_ptr, *up_ptr)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        *cameraPos_ptr -= cameraSpeed * (*front_ptr);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        *cameraPos_ptr += glm::normalize(glm::cross(*front_ptr, *up_ptr)) * cameraSpeed;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (fristMouseMove)
    {
        fristMouseMove = false;
    }

    float xoffset = (xpos - lastX) * 0.1;
    float yoffset = (ypos - lastY) * 0.1;
    lastX = xpos;
    lastY = ypos;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }
    if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }

    glm::vec3 local_front;
    local_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    local_front.y = sin(glm::radians(pitch));
    local_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    *front_ptr = glm::normalize(local_front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov_angle -= static_cast<float>(yoffset);
    if (fov_angle < 0.0f)
    {
        fov_angle = 0.0f;
    }
    if (fov_angle > 180.0f)
    {
        fov_angle = 180.0f;
    }
}