#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "stbi_images.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"

#include "spdlog/spdlog.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


int main(int argc, char* argv[]) {

#if 1
    // init spdlog configure
    spdlog::set_level(spdlog::level::debug);
#endif
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                          "LearnOpenGL",
                                          nullptr,
                                          nullptr);
    if (window == nullptr) {
        spdlog::critical("Failed to Create GLFW window");
        glfwTerminate();
        return  -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 窗口是否需要捕捉鼠标
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        spdlog::critical("Failed to Initialize GLAD");
        return -1;
    }

#ifdef ENABLE_DEPTH_TEST
    glEnable(GL_DEPTH_TEST);
#endif /* ENABLE_DEPTH_TEST */

    float quadVertices[] = {
        // 位置          // 颜色
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
        0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
        0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };

    glm::vec2 translations[100];

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    spdlog::info("generate vertex array object: {}", quadVAO);
    glGenBuffers(1, &quadVBO);
    spdlog::info("generate vertex buffer object: {}", quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    // 位置
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // 颜色
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // init translations;
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2) {
        for (int x = -10; x < 10; x += 2) {
            glm::vec2 translation;
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index] = translation;
            ++index;
        }
    }

    // 如果不启用几何变换
    Shader shader{"../res/glsl/vertex.vs", "../res/glsl/fragment.vs"};

    shader.use();

    for (uint32_t i = 0; i < 100; i++) {
        std::stringstream ss;
        std::string strIndex;
        ss << i;
        strIndex = ss.str();
        // 使用 for 循环去为 offsets 的每一个位置设置对应的位移向量
        shader.setVec2(("offsets[" + strIndex + "]").c_str(), translations[i]);
    }

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // 处理键盘的输入
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.69f, 0.77f, 0.87f, 1.0f);

        glBindVertexArray(quadVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}