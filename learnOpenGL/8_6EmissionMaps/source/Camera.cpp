#include "Camera.h"
#include <stdio.h>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    :   mFront(glm::vec3(0.0f, 0.0f, -1.0f)),
        mMovementSpeed(SPEED),
        mMouseSensitivity(SENSITIVITY),
        mZoom(ZOOM)
{
    mPosition = position;
    mWorldUp = up;
    mYaw = yaw;
    mPitch = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
{
    mPosition = glm::vec3(posX, posY, posZ);
    mWorldUp = glm::vec3(upX, upY, upZ);
    mYaw = yaw;
    mPitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    // 在不动鼠标的情况下，摄像机始终平行的看向 z 轴的负方向
    return glm::lookAt(mPosition, mPosition + mFront, mUp);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = mMovementSpeed * deltaTime;
    switch(direction)
    {
        case Camera_Movement::FORWARD:
            mPosition += mFront * velocity;
            break;
        case Camera_Movement::BACKWARD:
            mPosition -= mFront * velocity;
            break;
        case Camera_Movement::LEFT:
            mPosition -= mRight * velocity;
            break;
        case Camera_Movement::RIGHT:
            mPosition += mRight * velocity;
            break;
        case Camera_Movement::UP:
            mPosition += mWorldUp * velocity;
            break;
        case Camera_Movement::DOWN:
            mPosition -= mWorldUp * velocity;
            break;
        default:
            break;
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= mMouseSensitivity;
    yoffset *= mMouseSensitivity;

    mYaw += xoffset;
    mPitch += yoffset;

    if (constrainPitch)
    {
        if (mPitch > 89.0f)
        {
            mPitch = 89.0f;
        }
        if (mPitch < -89.0f)
        {
            mPitch = -89.0f;
        }
    }

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    mZoom -= yoffset;
    if (mZoom < 1.0f)
    {
        mZoom = 1.0f;
    }
    if (mZoom > 45.0f)
    {
        mZoom = 45.0f;
    }
}

void Camera::updateCameraVectors()
{
    glm::vec3 localFront;
    localFront.x = cos( glm::radians(mYaw) ) * cos( glm::radians(mPitch) );
    localFront.y = sin( glm::radians(mPitch) );
    localFront.z = sin( glm::radians(mYaw) ) * cos( glm::radians(mPitch) );

    mFront = glm::normalize(localFront);

    mRight = glm::normalize( glm::cross(mFront, mWorldUp) );
    mUp = glm::normalize( glm::cross(mRight, mFront) );
}