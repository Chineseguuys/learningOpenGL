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

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const std::string NANOSUIT_PATH = "../res/nanosuit/nanosuit.obj";

// 相机📷
Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};
float  lastX = static_cast<float>(SCR_WIDTH / 2.0f);
float  lastY = static_cast<float>(SCR_HEIGHT / 2.0f);
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(int argc, char* argv[]) {
#if 1
    // init spdlog configure
    spdlog::set_level(spdlog::level::trace);
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
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
#ifdef ENABLE_CAPTURE_CURSOR
    // 窗口是否需要捕捉鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif /* ENABLE_CAPTURE_CURSOR */
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        spdlog::error("Failed to Initialize GLAD");
        return -1;
    }

#ifdef ENABLE_DEPTH_TEST
	//是否允许深度测试
    glEnable(GL_DEPTH_TEST);
#endif /* ENABLE_DEPTH_TEST */

#ifdef ENABLE_POINT_SIZE
    // 控制默认绘制点的大小
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(5.0f);  // 绘制点时，控制点的大小
    glLineWidth(5.0f);  // 绘制线时，控制线的宽度
#endif /* ENABLE_POINT_SIZE */

    // 编译 shader
    // 几何变换，点绘制转化为线的绘制和三角形的绘制
    Shader shader{"../res/glsl/vertex_normal.glsl",
				  "../res/glsl/fragment_normal.glsl"};
	Shader normalShader {"../res/glsl/vertex.glsl",
						 "../res/glsl/fragment.glsl",
						 "../res/glsl/geomery.glsl"
	};

	// 加载模型
	stbi_set_flip_vertically_on_load(true);
	Model nanosuit(NANOSUIT_PATH.c_str());

    // render loop
    while (!glfwWindowShouldClose(window)) {
		// 每一帧的时间
		float currentTime = static_cast<float>(glfwGetTime());
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

        // 处理键盘的输入
        processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
#ifdef ENABLE_DEPTH_TEST
        // 开启了深度测试，需要设置 mask
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
        glClear(GL_COLOR_BUFFER_BIT);
#endif

		glm::mat4 projection = glm::perspective(
				glm::radians(45.0f),
				static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
				0.1f,
				100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0);

		// 绘制模型
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);
		nanosuit.Draw(shader);

		// 绘制模型的法线
		normalShader.use();
		normalShader.setMat4("projection", projection);
		normalShader.setMat4("view", view);
		normalShader.setMat4("model", model);
		nanosuit.Draw(normalShader);

		// 交换缓冲区
		glfwSwapBuffers(window);
		glfwPollEvents();
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
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = ypos - lastY;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}