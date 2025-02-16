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
// for matrix print
#include "spdlog/fmt/ostr.h"

// 添加 glm::mat4 可以通过spdlog 进行打印
template <>
struct fmt::formatter<glm::mat4> {
    constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin()) {
        // 解析格式规范（如果有的话）
        return ctx.end();
    }

    template <typename FormatContext>
    auto format(const glm::mat4& mat, FormatContext& ctx) const -> decltype(ctx.out()) {
        // 格式化矩阵为字符串
        return fmt::format_to(ctx.out(),
            "[[{}, {}, {}, {}],\n"
            " [{}, {}, {}, {}],\n"
            " [{}, {}, {}, {}],\n"
            " [{}, {}, {}, {}]]",
            mat[0][0], mat[0][1], mat[0][2], mat[0][3],
            mat[1][0], mat[1][1], mat[1][2], mat[1][3],
            mat[2][0], mat[2][1], mat[2][2], mat[2][3],
            mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
    }
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void titleFps(GLFWwindow* win);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
unsigned int loadTexture(char const * path, bool gammaCorrection);
void renderQuad();

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// static data
static float vertices[] = {
    // Positions          // normals           // Texture Coords
    0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // Top Right
    0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // Bottom Right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom Left
    -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f   // Top Left
};

static unsigned int indexs[] = {
    0, 1, 3, // First Triangle
    1, 2, 3  // Second Triangle
};

GLboolean useNormalMap = GL_TRUE;
GLboolean normalMapKeyPressed = GL_FALSE;
static const GLint NORMAL_MAP_KEY = GLFW_KEY_SPACE;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = static_cast<float>(SCR_WIDTH) / 2.0f;
float lastY = static_cast<float>(SCR_HEIGHT) / 2.0;
bool firstMouse = true;
static const float nearPlane = 0.1f;
static const float farPlane = 100.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Light
static const glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 5.0f);
static const glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

int main(int argc, char* argv[]) {

#if 1
    // init spdlog configure
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%P:%t][%Y-%m-%d %H:%M:%S.%e] [%^%-8l%$] %v");
#endif
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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
    } else {
        spdlog::info("Success Create GLFW Window");
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // 窗口是否需要捕捉鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 所有的 opengl 的操作都需要在下面 gladLoadLoader 之后去执行
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        spdlog::critical("Failed to Initialize GLAD");
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    unsigned int diffureMap = loadTexture("../res/texture/brickwall.jpg", false);
    unsigned int normalMap = loadTexture("../res/texture/brickwall_normal.jpg", false);

    Shader shader("../res/glsl/normal_mapping.vs", "../res/glsl/normal_mapping.fs");

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexs), indexs, GL_STATIC_DRAW);

    // Location 0
    // 顶点数据的性质 注意颜色和顶点的偏移量和 location
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    shader.use();
    shader.setInt("diffuseMap", 0);
    shader.setInt("normalMap", 1);

    // render loop
    while (!glfwWindowShouldClose(window)) {

        // pre-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 处理键盘的输入
        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(camera.mZoom, static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), nearPlane, farPlane);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("lightColor", lightColor);
        shader.setVec3("viewPos", camera.mPosition);
        shader.setBool("useNormalMap", useNormalMap);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffureMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // 在 glfw 的标题栏中显示当前渲染的帧率
        titleFps(window);
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    if (glfwGetKey(window, NORMAL_MAP_KEY) == GLFW_PRESS && !normalMapKeyPressed) {
        useNormalMap = !useNormalMap;
        spdlog::debug("change use normal map to status: {}", useNormalMap);
        normalMapKeyPressed = GL_TRUE;
    }
    if (glfwGetKey(window, NORMAL_MAP_KEY) == GLFW_RELEASE) {
        normalMapKeyPressed = GL_FALSE;
    }
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

void titleFps(GLFWwindow* win) {
    static double time0 = glfwGetTime();
    static double time1;
    static double dt;
    static int dframe = 1;
    static std::stringstream info;

    time1 = glfwGetTime();
    dframe++;
    if ((dt = time1 - time0) >= 1.0) {
        info.precision(1);
        info << "LearnOpenGL" << " " << std::fixed << dframe / dt << " FPS";
        glfwSetWindowTitle(win, info.str().c_str());
        info.str("");
        time0 = time1;
        dframe = 0;
    }
}

unsigned int loadTexture(char const * path, bool gammaCorrection)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        if (nrComponents == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        spdlog::debug("Texture failed to load at path: {}", path);
        stbi_image_free(data);
    }

    return textureID;
}