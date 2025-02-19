//
// Created by yanjiangha on 2024/5/26.
//

#include "processInput.h"

ProcessInput::ProcessInput()
    : mDeltaTime(0.0f),
    mLastRenderTime(0.0f),
    mCamera(nullptr),
    mWindow(nullptr)
{
}

ProcessInput::ProcessInput(Camera *camera, GLFWwindow *win)
    : mDeltaTime(0.0f), mLastRenderTime(0.0f),
    mCamera(camera), mWindow(win)
{
}

void ProcessInput::setCamera(Camera *cam) {
    if (cam != nullptr && cam != mCamera) {
        this->mCamera = cam;
    }
}

void ProcessInput::setGLFWwindow(GLFWwindow *win) {
    if (win != nullptr && win != mWindow) {
        this->mWindow = win;
    }
}

void ProcessInput::dealWithInput() {
    double currentRenderTime = glfwGetTime();
    mDeltaTime = currentRenderTime - mLastRenderTime;
    mLastRenderTime = currentRenderTime;

    if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(mWindow, true);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS) {
        mCamera->ProcessKeyboard(Camera_Movement::FORWARD, mDeltaTime);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS) {
        mCamera->ProcessKeyboard(Camera_Movement::LEFT, mDeltaTime);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS) {
        mCamera->ProcessKeyboard(Camera_Movement::BACKWARD, mDeltaTime);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS) {
        mCamera->ProcessKeyboard(Camera_Movement::RIGHT, mDeltaTime);
    }
}
