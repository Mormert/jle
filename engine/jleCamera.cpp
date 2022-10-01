// Copyright (c) 2022. Johan Lind
#include "jleCamera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <plog/Log.h>

jleCamera::jleCamera(jleCameraProjection projection) {
    _projectionType = projection;

    if (projection == jleCameraProjection::Orthographic) {
        SetOrthographicProjection(512, 512, 1000.f, -1000.f);
    }
    else {
        SetPerspectiveProjection(90.f, 512, 512, 1000.f, -1000.f);
    }
}

void jleCamera::SetPerspectiveProjection(float fov,
                                         uint32_t screenWidth,
                                         uint32_t screenHeight,
                                         float farPlane,
                                         float nearPlane) {
    _projectionMatrix = glm::perspective(
        fov, (float)screenWidth / (float)screenHeight, nearPlane, farPlane);
}

void jleCamera::SetOrthographicProjection(uint32_t screenWidth,
                                          uint32_t screenHeight,
                                          float farPlane,
                                          float nearPlane) {
    _projectionMatrix = glm::ortho(static_cast<float>(0.f),
                                   static_cast<float>(screenWidth),
                                   static_cast<float>(screenHeight),
                                   static_cast<float>(0.f),
                                   nearPlane,
                                   farPlane);
}

glm::mat4 jleCamera::GetProjectionViewMatrix() const {
    switch (_projectionType) {
    case jleCameraProjection::Perspective: {
        const glm::mat4 view = glm::lookAt(_position, _position + _front, _up);
        return _projectionMatrix * view;
    }
    case jleCameraProjection::Orthographic: {
        glm::mat4 view{1.f};
        view = glm::translate(view, _position);
        // view = glm::rotate(view, glm::radians(_cameraRotationDegrees),
        // glm::vec3{}) view = glm::scale(view, glm::vec3{1.f});
        // TODO: Support camera rotation, zooming?
        return _projectionMatrix * view;
    }
    }

    return glm::identity<glm::mat4>();
}

void jleCamera::SetPerspectiveMouseSensitivity(float sensitivity) {
    _mouseSensitivity = sensitivity;
}

void jleCamera::ApplyPerspectiveMouseMovementDelta(glm::vec2 delta) {
    delta *= _mouseSensitivity;
    _yaw = glm::mod(_yaw + delta.x, 360.0f);
    _pitch += delta.y;

    _pitch = glm::clamp(_pitch, -89.f, 89.f);

    CalculatePerspectiveVectors();
}

// TODO:
// https://gitlab.com/muffinman007/OpenGL_360_Camera_Quarternion/blob/master/Camera.h

void jleCamera::CalculatePerspectiveVectors() {
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

void jleCamera::MovePerspectiveForward(float speed) {
    _position += _front * speed;
}

void jleCamera::MovePerspectiveBackward(float speed) {
    _position -= _front * speed;
}

void jleCamera::MovePerspectiveRight(float speed) {
    _position += _right * speed;
}

void jleCamera::MovePerspectiveLeft(float speed) {
    _position -= _right * speed;
}

void jleCamera::MovePerspectiveUp(float speed) { _position += _up * speed; }

void jleCamera::MovePerspectiveDown(float speed) { _position -= _up * speed; }
