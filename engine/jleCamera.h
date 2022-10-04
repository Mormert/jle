// Copyright (c) 2022. Johan Lind

#pragma once

#include <glm/glm.hpp>

enum class jleCameraProjection { Orthographic, Perspective };

struct jleCamera {

    explicit jleCamera() = default;

    explicit jleCamera(jleCameraProjection projection);

    jleCameraProjection _projectionType = jleCameraProjection::Orthographic;

    glm::vec3 _position{0.f, 24.f, 0.f};

    glm::mat4 _projectionMatrix{};

    // For perspective projection
    glm::vec3 _front{0.0f, 0.0f, -1.0f};
    glm::vec3 _up{0.0f, 1.0f, 0.0f};
    glm::vec3 _right{};
    const glm::vec3 _worldUp{0.0f, 1.0f, 0.0f};
    float _yaw{-90.f};
    float _pitch{0.f};
    float _mouseSensitivity{0.001f};

    // For orthographic projection
    float _cameraRotationDegrees{};

    void perspectiveProjection(float fov,
                               uint32_t screenWidth,
                               uint32_t screenHeight,
                               float farPlane,
                               float nearPlane);

    void orthographicProjection(uint32_t screenWidth,
                                uint32_t screenHeight,
                                float farPlane,
                                float nearPlane);

    [[nodiscard]] glm::mat4 projectionViewMatrix() const;

    void perspectiveMouseSensitivity(float sensitivity);

    void applyPerspectiveMouseMovementDelta(glm::vec2 delta);

    void calculatePerspectiveVectors();

    void movePerspectiveForward(float speed);

    void movePerspectiveBackward(float speed);

    void movePerspectiveRight(float speed);

    void movePerspectiveLeft(float speed);

    void movePerspectiveUp(float speed);

    void movePerspectiveDown(float speed);

    float _x = 0.f, _y = 0.f;
    float _xNoOffset = 0.f, _yNoOffset = 0.f;

    int _cameraWidth{}, _cameraHeight{};

    [[nodiscard]] int32_t intX() const { return int32_t(_x); }

    [[nodiscard]] int32_t intY() const { return int32_t(_y); }
};
