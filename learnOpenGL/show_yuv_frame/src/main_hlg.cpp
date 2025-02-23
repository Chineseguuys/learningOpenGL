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
#include <vector>
#include <algorithm>

// ------

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void titleFps(GLFWwindow* win);
unsigned int createTexture(GLenum type, int width, int height);
bool readYUV420p(const std::string& filePath, std::vector<unsigned char>& y, std::vector<unsigned char>& u, std::vector<unsigned char>& v, int width, int height);
bool readYUV420p10le(const std::string& filePath, std::vector<uint16_t>& y_plane, std::vector<uint16_t>& u_plane, std::vector<uint16_t>& v_plane, int width, int height);
// ------

enum COLOR_SPACE {
    BT_601    = 0,
    BT_709    = 1,
    BT_2020NC = 2, 
};

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// choose color
int colorSpace = COLOR_SPACE::BT_601;

// swap uv?
bool swapUV = false;

// nonlinear -> linear
bool nonlinear2Linear = false;

// transpose the matrix in fragment shader
bool transposeYuv2RgbMatrix = false;

// vertex and texcoords
static const float vertices[] = {
    // 位置       // 纹理坐标
    -1.0f,  1.0f, 0.0, 0.0f, 0.0f,
    -1.0f, -1.0f, 0.0, 0.0f, 1.0f,
     1.0f, -1.0f, 0.0, 1.0f, 1.0f,
     1.0f,  1.0f, 0.0, 1.0f, 0.0f
};
static const unsigned int indices[] = {0, 1, 2, 0, 2, 3};

// video frame width and height
static const int video_width = 3840;
static const int video_height = 2160;
std::vector<uint16_t> y_plane(video_width * video_height);
std::vector<uint16_t> u_plane(video_width / 2 * video_height / 2);
std::vector<uint16_t> v_plane(video_height / 2 * video_width / 2);

// yuv420p
//YYYYYY
//UUUUUU
//VVVVVV

// blue white yuv file with resolution 640x360
// normal yuv file with resolution 1920x1080
static const std::string yuv_file_path = "../res/yuv/hlg10_3840x2160_yuv420p10le.yuv";

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
    //glfwSetCursorPosCallback(window, mouse_callback);

    // 窗口是否需要捕捉鼠标 不捕捉
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 所有的 opengl 的操作都需要在下面 gladLoadLoader 之后去执行
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        spdlog::critical("Failed to Initialize GLAD");
        return -1;
    }


    // load yuv data
    readYUV420p10le(yuv_file_path, y_plane, u_plane, v_plane, video_width, video_height);
    // generate texture
    GLuint texY = createTexture(GL_R16, video_width, video_height);
    GLuint texU = createTexture(GL_R16, video_width / 2, video_height / 2);
    GLuint texV = createTexture(GL_R16, video_width / 2, video_height / 2);
    // upload texture data
    glBindTexture(GL_TEXTURE_2D, texY);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, video_width, video_height, GL_RED, GL_UNSIGNED_SHORT, y_plane.data());
    glBindTexture(GL_TEXTURE_2D, texU);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, video_width / 2, video_height / 2, GL_RED, GL_UNSIGNED_SHORT, u_plane.data());
    glBindTexture(GL_TEXTURE_2D, texV);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, video_width / 2, video_height / 2, GL_RED, GL_UNSIGNED_SHORT, v_plane.data());

    // VAO VBO create
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Shader shader{"../res/glsl/show_yuv_frame_hlg.vs", "../res/glsl/show_yuv_frame_hlg.fs"};
    shader.use();
    shader.setInt("texY", 0);
    shader.setInt("texU", 1);
    shader.setInt("texV", 2);

    // enable depth test
    // glEnable(GL_DEPTH_TEST);

    // render loop
    while (!glfwWindowShouldClose(window)) {

        // pre-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());

        // 处理键盘的输入
        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setInt("whichMatrix", colorSpace);
        shader.setBool("swapUV", swapUV);
        shader.setBool("transposeYuv2Rgb", transposeYuv2RgbMatrix);
        shader.setBool("nonlinear2Linear", nonlinear2Linear);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texY);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texU);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texV);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // 在 glfw 的标题栏中显示当前渲染的帧率
        titleFps(window);
    }

    // recycle the recource created by gpu
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &texY);
    glDeleteTextures(1, &texU);
    glDeleteTextures(1, &texV);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    colorSpace = COLOR_SPACE::BT_2020NC;
    swapUV = false;
    transposeYuv2RgbMatrix = false;
    nonlinear2Linear = false;
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        colorSpace = COLOR_SPACE::BT_601;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        colorSpace = COLOR_SPACE::BT_709;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        colorSpace = COLOR_SPACE::BT_2020NC;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        swapUV = true;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        transposeYuv2RgbMatrix = true;
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        nonlinear2Linear = true;
    }
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
        info << "show yuv" << " " << std::fixed << dframe / dt << " FPS";
        glfwSetWindowTitle(win, info.str().c_str());
        info.str("");
        time0 = time1;
        dframe = 0;
    }
}

unsigned int createTexture(GLenum type, int width, int height) {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexStorage2D(GL_TEXTURE_2D, 1, type, width, height);

    return tex;
}

bool readYUV420p(const std::string& filePath, std::vector<unsigned char>& y, std::vector<unsigned char>& u, std::vector<unsigned char>& v, int width, int height) {
    const int y_size = width * height;
    const int uv_size = (width / 2) * (height / 2);
    const int total_size = y_size + 2 * uv_size;

    spdlog::info("{}: y_size = {}, uv_size = {}", __func__, y_size, uv_size);

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        spdlog::error("{}: Error open file {}", __func__, filePath.c_str());
        return false;
    }

    file.seekg(0, std::ios::end);
    if (file.tellg() != total_size) {
        spdlog::error("{}: Invalid file size. Expected: {}, Actual: {}", __func__, total_size, static_cast<int>(file.tellg()));
        return false;
    }

    file.seekg(0, std::ios::beg);

    // read full frame data
    std::vector<unsigned char> frame_data(total_size);
    file.read(reinterpret_cast<char*>(frame_data.data()), total_size);

    std::copy(frame_data.begin(), frame_data.begin() + y_size, y_plane.begin());
    std::copy(frame_data.begin() + y_size, frame_data.begin() + y_size + uv_size, u_plane.begin());
    std::copy(frame_data.begin() + y_size + uv_size, frame_data.end(), v_plane.begin());

    auto y_minmax = std::minmax_element(y_plane.begin(), y_plane.end());
    auto u_minmax = std::minmax_element(u_plane.begin(), u_plane.end());
    auto v_minmax = std::minmax_element(v_plane.begin(), v_plane.end());

    spdlog::debug("{}: y min value = {}, max value = {}", __func__, *y_minmax.first, *y_minmax.second);
    spdlog::debug("{}: u min value = {}, max value = {}", __func__, *u_minmax.first, *u_minmax.second);
    spdlog::debug("{}: v min value = {}, max value = {}", __func__, *v_minmax.first, *v_minmax.second);

    return true;
}

bool readYUV420p10le(const std::string& filePath, std::vector<uint16_t>& y_plane, std::vector<uint16_t>& u_plane, std::vector<uint16_t>& v_plane, int width, int height) {
    const int y_size = width * height;
    const int uv_size = (width / 2) * (height / 2);
    const int total_size = (y_size + 2 * uv_size) * 2;

    spdlog::info("{}: y_size = {}, uv_size = {}", __func__, y_size, uv_size);

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        spdlog::error("{}: Error open file {}", __func__, filePath.c_str());
        return false;
    }

    file.seekg(0, std::ios::end);
    if (file.tellg() != total_size) {
        spdlog::error("{}: Invalid file size. Expected: {}, Actual: {}", __func__, total_size, static_cast<int>(file.tellg()));
        return false;
    }

    file.seekg(0, std::ios::beg);

    spdlog::trace("{}: read from file now", __func__);
    // read full frame data
    std::vector<uint16_t> frame_data(y_size + uv_size * 2);
    file.read(reinterpret_cast<char*>(frame_data.data()), total_size);

    spdlog::trace("{}: copy content to y u v part", __func__);

    std::copy(frame_data.begin(), frame_data.begin() + y_size, y_plane.begin());
    std::copy(frame_data.begin() + y_size, frame_data.begin() + y_size + uv_size, u_plane.begin());
    std::copy(frame_data.begin() + y_size + uv_size, frame_data.end(), v_plane.begin());

#ifndef USING_BIG_ENDIAN
    spdlog::info("{}: change little endian to big endian", __func__);
    for (auto& val : y_plane) {
        val = static_cast<uint16_t>((val & 0xFF00) >> 8) |
            static_cast<uint16_t>((val & 0x00FF) << 8);
    }

    for (auto& val : u_plane) {
        val = static_cast<uint16_t>((val & 0xFF00) >> 8) |
            static_cast<uint16_t>((val & 0x00FF) << 8);
    }

    for (auto& val : v_plane) {
        val = static_cast<uint16_t>((val & 0xFF00) >> 8) |
            static_cast<uint16_t>((val & 0x00FF) << 8);
    }
#endif /* USING_BIG_ENDIAN */

    auto y_minmaxElements = std::minmax_element(y_plane.begin(), y_plane.end());
    auto u_minmaxElements = std::minmax_element(u_plane.begin(), u_plane.end());
    auto v_minmaxElements = std::minmax_element(v_plane.begin(), v_plane.end());

    spdlog::info("{}: y min value={}, max value={}", __func__, *y_minmaxElements.first, *y_minmaxElements.second);
    spdlog::info("{}: u min value={}, max value={}", __func__, *u_minmaxElements.first, *u_minmaxElements.second);
    spdlog::info("{}: v min value={}, max value={}", __func__, *v_minmaxElements.first, *v_minmaxElements.second);

    spdlog::trace("{}: success return", __func__);

    return true;
}