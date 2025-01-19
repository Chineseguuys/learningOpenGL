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
        spdlog::error("Failed to Create GLFW window");
        glfwTerminate();
        return  -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 窗口是否需要捕捉鼠标
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        spdlog::error("Failed to Initialize GLAD");
        return -1;
    }

#ifdef ENABLE_DEPTH_TEST
    glEnable(GL_DEPTH_TEST);
#endif /* ENABLE_DEPTH_TEST */

#ifdef ENABLE_POINT_SIZE
    // 控制默认绘制点的大小
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(5.0f);  // 绘制点时，控制点的大小
    glLineWidth(5.0f);  // 绘制线时，控制线的宽度
#endif /* ENABLE_POINT_SIZE */

    float vertices[] = {
        // four point position
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f
    };

    float points[] = {
        // position vec2[without z]      color vec3
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
        0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
    };

    // 编译 shader
    // 几何变换，点绘制转化为线的绘制和三角形的绘制
    Shader shader{"../res/glsl/vertex.vs",
                  "../res/glsl/fragment.vs",
                  "../res/glsl/geomery.vs"
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    spdlog::debug("gen vertex array with id {}, gen buffer with id {}", VAO, VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    // for position
    glVertexAttribPointer(0, 2,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(float),
                          (void*)0);
    // for color
	glVertexAttribPointer(1, 3,
						  GL_FLOAT,
						  GL_FALSE,
						  5 * sizeof(float),
						  (void*)(2 * sizeof(float)));
	// enable vertex attrib
	// enable aPos
	glEnableVertexAttribArray(0);
	// enable aColor
	glEnableVertexAttribArray(1);

    shader.use();

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // 处理键盘的输入
        processInput(window);
#ifdef ENABLE_DEPTH_TEST
        // 开启了深度测试，需要设置 mask
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
        glClear(GL_COLOR_BUFFER_BIT);
#endif
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        shader.use();
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_POINTS, 0, 4);
        // swap buffer and poll ui events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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