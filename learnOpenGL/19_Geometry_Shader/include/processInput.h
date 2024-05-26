//
// Created by yanjiangha on 2024/5/26.
//

#ifndef ADVANCED_GLSL_PROCESSINPUT_H
#define ADVANCED_GLSL_PROCESSINPUT_H


#include "camera.h"

class ProcessInput {
private:
    double mDeltaTime;
    double mLastRenderTime;
    Camera* mCamera;
    GLFWwindow* mWindow;

public:
    ProcessInput();
    ProcessInput(Camera* camera, GLFWwindow* win);
    void setCamera(Camera* cam);
    void setGLFWwindow(GLFWwindow* win);
    void dealWithInput();
};


#endif //ADVANCED_GLSL_PROCESSINPUT_H
