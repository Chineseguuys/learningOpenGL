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

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

const char *VERTEX_SHADER_FILEPATH = "res/glsl/shader.vs";
const char *FRAGMENT_SHADER_FILEPATH = "res/glsl/shader.fs";
const char *TEXTURE1_IMAGE_FILEPATH = "res/textures/wall.jpg";
const char *TEXTURE2_IMAGE_FILEPATH = "res/textures/container.jpg";

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
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

    float *texcoord_pointer[] = {
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
        &vertices[179]};

    // 后面再尝试精简这些顶点

    // 创建变换矩阵 单位矩阵
    const glm::mat4 base = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0f, 0.0f));

    // 模型矩阵 view矩阵 投影矩阵
    glm::mat4 view = glm::translate(base, glm::vec3(0.0f, 0.0f, -3.0f));

    // 透视投影
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                            static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                                            0.1f, 100.0f);

    glm::mat4 orth_projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);

    // 围绕 x 轴进行旋转
    glm::mat4 model = glm::rotate(base, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

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

    // 创建 Texture1
    unsigned int texture1;
    glGenTextures(1, &texture1);
    // 一定要先 bind 之后，设置的属性才可以生效
    glBindTexture(GL_TEXTURE_2D, texture1);
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载一张图片
    int width, height, nrChannels;
    unsigned char *image = stbi_load(TEXTURE1_IMAGE_FILEPATH, &width, &height, &nrChannels, 0);

    if (image != NULL)
    {
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        /**
         * @brief 这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。因为上面的一个函数相应的值为 0， 所以这里不会自动生成多级渐远的纹理
         */
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载一张图片
    image = stbi_load(TEXTURE2_IMAGE_FILEPATH, &width, &height, &nrChannels, 0);

    if (image != NULL)
    {
        printf("image size is [%d, %d, %d]\n", width, height, nrChannels);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    // 纹理已经设置，图像可以删除了。
    stbi_image_free(image);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    shader.use();

    const int TEXTURES_INT[] = {0, 1};
    shader.setInt("Texture1", TEXTURES_INT[0]);
    shader.setInt("Texture2", TEXTURES_INT[1]);
    shader.setFloat("mixParameter", 0.5);

    shader.setMatrix4f("model", model);
    shader.setMatrix4f("view", view);
#ifndef USING_ORTH
    shader.setMatrix4f("projection", projection);
#else
    shader.setMatrix4f("projection", orth_projection);
#endif

#ifdef __USING_IMGUI__
    // ImGui
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
#endif

    float mix_parameter = 0.5f;
    float texture_coord = 1.0f;
    float rotate_angle[3] = {-55.0f, 0.0f, 0.0f};
    float fov_angle = 45.0f;

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

            ImGui::Text("change two textures mix parameter"); // Display some text (you can use a format strings too)
            ImGui::SliderFloat("mix param", &mix_parameter, 0.0f, 1.0f);
            ImGui::SliderFloat("texture coord", &texture_coord, 0.0f, 4.0f);
            ImGui::SliderFloat3("rotate xyz", rotate_angle, -180.0f, 180.0f);
            ImGui::SliderFloat("fov", &fov_angle, 0.0f, 180.0f);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
#endif /* __USING_IMGUI__ */

        for (float *v : texcoord_pointer)
        {
            *v = texture_coord;
        }

        projection = glm::perspective(glm::radians(fov_angle),
                                      static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                                      0.1f, 100.0f);

        // 围绕 xyz 轴进行旋转
        glm::mat4 rx = glm::rotate(base, glm::radians(rotate_angle[0]), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 ry = glm::rotate(base, glm::radians(rotate_angle[1]), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rz = glm::rotate(base, glm::radians(rotate_angle[2]), glm::vec3(0.0f, 0.0f, 1.0f));
        model = rz * ry * rx;

        // 是否是正确的方式 需要不断的更新
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        // 激活特定的插槽
        // 绑定 texture
        glActiveTexture(GL_TEXTURE0 + TEXTURES_INT[0]);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE0 + TEXTURES_INT[1]);
        glBindTexture(GL_TEXTURE_2D, texture2);

        shader.use();
        shader.setFloat("mixParameter", mix_parameter);
        shader.setMatrix4f("model", model);
#ifndef USING_ORTH
        shader.setMatrix4f("projection", projection);
#else
        shader.setMatrix4f("projection", orth_projection);
#endif
        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 36);

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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}