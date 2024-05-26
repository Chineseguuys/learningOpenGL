#define __USING_IMGUI__

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

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

const char* VERTEX_SHADER_FILEPATH = "res/glsl/shader.vs";
const char* FRAGMENT_SHADER_FILEPATH = "res/glsl/shader.fs";
const char* TEXTURE1_IMAGE_FILEPATH = "res/textures/wall.jpg";
const char* TEXTURE2_IMAGE_FILEPATH = "res/textures/container.jpg";

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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader shader(VERTEX_SHADER_FILEPATH, FRAGMENT_SHADER_FILEPATH);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // Positions          // Colors           // Texture Coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // Top Right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom Right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Bottom Left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,  0.0f, 1.0f  // Top Left 
    };

    unsigned int indexs[] = {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };

    // 指向纹理坐标几个关键坐标的指针。方便通过 imgui 来方便的调节纹理坐标的值
    float* vertices_pointer[] = {
        &vertices[6],  &vertices[7],
        &vertices[14],
        &vertices[31]
    };


    // 创建变换矩阵 单位矩阵
    const glm::mat4 base = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0f, 0.0f));

    // 模型矩阵 view矩阵 投影矩阵
    glm::mat4 view  = glm::translate(base, glm::vec3(0.0f, 0.0f, -3.0f));

    // 透视投影
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                0.1f, 100.0f);

    // 围绕 x 轴进行旋转
    glm::mat4 model = glm::rotate(base, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexs), indexs, GL_STATIC_DRAW);

    // Location 0
    // 顶点数据的性质 注意颜色和顶点的偏移量和 location
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);   // for location 0
    glEnableVertexAttribArray(1);   // for location 1
    glEnableVertexAttribArray(2);   // for location 2


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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
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

    float mix_parameter = 0.5f;
    float texCorrd_range = 1.0f;
    float fov_value = 45.0f;
    float view_xyz[3] = {0.0f, 0.0f, -3.0f};
    float rotate_xaxis = -55.0f;

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

#ifdef __USING_IMGUI__
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::Begin("TEST TEXTURE MIX");

            ImGui::Text("change two textures mix parameter");               // Display some text (you can use a format strings too)
            ImGui::SliderFloat("mix para", &mix_parameter, 0.0f, 1.0f);
            ImGui::SliderFloat("texcoord range", &texCorrd_range, 0.0f, 8.0f);
            ImGui::SliderFloat("fov value", &fov_value, 0.0f, 180.0f);
            ImGui::SliderFloat3("view xyz", view_xyz, -10.0f, 10.0f);
            ImGui::SliderFloat("rotate bebind x", &rotate_xaxis, -180.0f, 180.0f);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
#endif /* __USING_IMGUI__ */

        // 作用与纹理坐标
        for (float* v : vertices_pointer) {
            *v = texCorrd_range;
        }

        projection = glm::perspective(glm::radians(fov_value), 
                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 
                0.1f, 100.0f);
        view  = glm::translate( base, glm::vec3(view_xyz[0], view_xyz[1], view_xyz[2]) );
        model = glm::rotate(base, glm::radians(rotate_xaxis), glm::vec3(1.0f, 0.0f, 0.0f));

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
        shader.setMatrix4f("model", model);
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);

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
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}