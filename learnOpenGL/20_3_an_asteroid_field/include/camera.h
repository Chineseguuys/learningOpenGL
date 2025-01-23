//
// Created by yanjianghan on 2023/8/27.
//

#ifndef ASSIMP_MODEL_INCLUDE_CAMERA_H
#define ASSIMP_MODEL_INCLUDE_CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

enum Camera_Movement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN
};

const float YAW             = -90.0f;
const float PITCH           = 0.0f;
const float SPEED           = 7.25f; // 控制 camera 移动的速度
const float SENSITIVITY     = 0.1f;
const float ZOOM            = 45.0f;

class Camera {
public:
  glm::vec3 mPosition;
  glm::vec3 mFront;
  glm::vec3 mUp;
  glm::vec3 mRight;
  glm::vec3 mWorldUp;

  // 欧拉角
  float mYaw;
  float mPitch;
  // 相机选项
  float mMovementSpeed;
  float mMouseSensitivity;
  float mZoom;

  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
  glm::mat4 GetViewMatrix() const;
  void ProcessKeyboard(Camera_Movement direction, float deltaTime);
  void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
  void ProcessMouseScroll(float yoffset);

private:
  void updateCameraVectors();
};

#endif //ASSIMP_MODEL_INCLUDE_CAMERA_H
