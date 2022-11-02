// Copyright (c) 2022. Johan Lind
#include "jleCamera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <plog/Log.h>

jleCamera::jleCamera(jleCameraProjection projection) {
    if (projection == jleCameraProjection::Orthographic) {
        setOrthographicProjection(512, 512, 1000.f, -1000.f);
    }
    else {
        setPerspectiveProjection(90.f, 512, 512, 1000.f, -1000.f);
    }
}

void
jleCamera::setPerspectiveProjection(
    float fov, uint32_t screenWidth, uint32_t screenHeight, float farPlane, float nearPlane)
{
    _projectionMatrix =
        glm::perspective(glm::radians(fov), (float)screenWidth / (float)screenHeight, nearPlane, farPlane);

    // Flip the Y coordinate
    _projectionMatrix[1][1] *= -1.f;
}

void
jleCamera::setOrthographicProjection(uint32_t screenWidth, uint32_t screenHeight, float farPlane, float nearPlane)
{

    _projectionMatrix = glm::ortho(-((float)screenWidth / 2.0f),
                                   ((float)screenWidth / 2.0f),
                                   ((float)screenHeight / 2.0f),
                                   -((float)screenHeight / 2.0f),
                                   nearPlane,
                                   farPlane);
}

void
jleCamera::setViewMatrix(const glm::mat4 &view)
{
    _viewMatrix = view;
}

glm::mat4
jleCamera::getProjectionViewMatrix() const
{
    return _projectionMatrix * _viewMatrix;
}
glm::mat4
jleCamera::getProjectionMatrix() const
{
    return _projectionMatrix;
}

glm::mat4
jleCamera::getViewMatrix() const
{
    return _viewMatrix;
}

glm::vec3
jleCamera::getViewPosition() const
{
    return _viewMatrix[3];
}

glm::mat4
jleCameraSimpleFPVController::getLookAtViewMatrix() const
{
    return glm::lookAt(position, position + _front, _up);
}

void
jleCameraSimpleFPVController::setPerspectiveMouseSensitivity(float sensitivity)
{
    _mouseSensitivity = sensitivity;
}

void
jleCameraSimpleFPVController::applyPerspectiveMouseMovementDelta(glm::vec2 delta, float factor)
{
    delta *= _mouseSensitivity * factor;
    _yaw = glm::mod(_yaw + delta.x, 360.0f);
    _pitch += delta.y;

    _pitch = glm::clamp(_pitch, -89.f, 89.f);

    calculatePerspectiveVectors();
}

// TODO:
// https://gitlab.com/muffinman007/OpenGL_360_Camera_Quarternion/blob/master/Camera.h

void
jleCameraSimpleFPVController::calculatePerspectiveVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front = glm::normalize(front);
    _right = glm::normalize(glm::cross(_front, _worldUp));
    _up = glm::normalize(glm::cross(_right, _front));

    if (_pitch > 90.f || _pitch < 270.f) {
        _right *= -1;
    }
}

void
jleCameraSimpleFPVController::moveForward(float speed)
{
    position += _front * speed;
}

void
jleCameraSimpleFPVController::moveBackward(float speed)
{
    position -= _front * speed;
}

void
jleCameraSimpleFPVController::moveRight(float speed)
{
    position -= _right * speed;
}

void
jleCameraSimpleFPVController::moveLeft(float speed)
{
    position += _right * speed;
}

void
jleCameraSimpleFPVController::moveUp(float speed)
{
    position -= _up * speed;
}

void
jleCameraSimpleFPVController::moveDown(float speed)
{
    position += _up * speed;
}

void
jleCameraSimpleFPVController::move(glm::vec3 v)
{
    position += v;
}

void
jleCameraSimpleFPVController::backToOrigin()
{
    position = {0.f, 0.f, 0.f};
    _front = {0.0f, 0.0f, -1.0f};
    _up = {0.0f, 1.0f, 0.0f};
    _right = glm::vec3{};
    _yaw = -90.f;
    _pitch = 0.f;
    applyPerspectiveMouseMovementDelta({0, 0}, 0.f);
}
void
jleCameraSimpleFPVController::setPitch(float pitch)
{
    _pitch = pitch;
}
void
jleCameraSimpleFPVController::setYaw(float yaw)
{
    _yaw = yaw;
}
