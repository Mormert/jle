// Copyright (c) 2023. Johan Lind

#pragma once

#include <glm/glm.hpp>

enum class jleCameraProjection { Orthographic, Perspective };

struct jleCameraSimpleFPVController {

    void setPerspectiveMouseSensitivity(float sensitivity);

    void applyPerspectiveMouseMovementDelta(glm::vec2 delta, float factor);

    void moveForward(float speed);

    void moveBackward(float speed);

    void moveRight(float speed);

    void moveLeft(float speed);

    void moveUp(float speed);

    void moveDown(float speed);

    void move(glm::vec3 v);

    void setYaw(float yaw);

    void setPitch(float pitch);

    [[nodiscard]] glm::mat4 getLookAtViewMatrix() const;

    glm::vec3 position{0.f, 0.f, 0.f};

    void backToOrigin();

    void recalculateVectorsFromViewMatrix(const glm::mat4 &view);

private:
    void calculatePerspectiveVectors();

    glm::vec3 _front{0.0f, 0.0f, -1.0f};
    glm::vec3 _up{0.0f, 1.0f, 0.0f};
    glm::vec3 _right{};
    const glm::vec3 _worldUp{0.0f, 1.0f, 0.0f};
    float _yaw{-90.f};
    float _pitch{0.f};
    float _mouseSensitivity{0.001f};
};

struct jleCamera {

    explicit jleCamera() = default;

    explicit jleCamera(jleCameraProjection projection);

    void setPerspectiveProjection(
        float fov, uint32_t screenWidth, uint32_t screenHeight, float farPlane, float nearPlane);

    void setOrthographicProjection(uint32_t screenWidth, uint32_t screenHeight, float farPlane, float nearPlane);

    void setViewMatrix(const glm::mat4 &view, const glm::vec3 &position);

    void setBackgroundColor(const glm::vec3 &color);

    [[nodiscard]] glm::vec3 getPosition() const;

    [[nodiscard]] glm::vec3 getBackgroundColor() const;

    [[nodiscard]] glm::mat4 getProjectionViewMatrix() const;

    [[nodiscard]] glm::mat4 getProjectionMatrix() const;

    [[nodiscard]] glm::mat4 getViewMatrix() const;

    [[nodiscard]] jleCameraProjection getProjectionType() const;

private:
    jleCameraProjection _projectionType;
    glm::mat4 _projectionMatrix{1.f};
    glm::mat4 _viewMatrix{1.f};
    glm::vec3 _backgroundColor{0.1f, 0.1f, 0.1f};
    glm::vec3 _position;
};
