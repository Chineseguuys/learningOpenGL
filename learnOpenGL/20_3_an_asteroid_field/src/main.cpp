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
void titleFps(GLFWwindow* win);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 55.0f));
float lastX = static_cast<float>(SCR_WIDTH) / 2.0f;
float lastY = static_cast<float>(SCR_HEIGHT) / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

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
    glfwSetCursorPosCallback(window, mouse_callback);

    // 窗口是否需要捕捉鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        spdlog::critical("Failed to Initialize GLAD");
        return -1;
    }

#ifdef ENABLE_DEPTH_TEST
    glEnable(GL_DEPTH_TEST);
#endif /* ENABLE_DEPTH_TEST */

    // Shader
    Shader shader("../res/glsl/vertex.vs", "../res/glsl/fragment.vs");

    // Load Model
    Model rock("../res/models/rock/rock.obj");
    Model planet("../res/models/planet/planet.obj");

    unsigned int amount = 8000;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(glfwGetTime()));
    float radius = 25.0f;
    float offset = 2.5f;

    for (unsigned int i = 0; i < amount; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        float angle = static_cast<float>(i) / static_cast<float>(amount) * 360.0f;
        float displacement = (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius  + displacement;
        displacement = (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = static_cast<float>((rand() % 360));
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    // render loop
    while (!glfwWindowShouldClose(window)) {

        // pre-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 处理键盘的输入
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 project = glm::perspective(glm::radians(45.0f),
            static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
            0.1f,
            1000.0f
        );
        glm::mat4 view = camera.GetViewMatrix();

        shader.use();
        shader.setMat4("projection", project);
        shader.setMat4("view", view);

        // draw planet
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        shader.setMat4("model", model);
        planet.Draw(shader);

        // draw meteorites
        for (unsigned int i = 0; i < amount; i++) {
            shader.setMat4("model", modelMatrices[i]);
            rock.Draw(shader);
        }

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