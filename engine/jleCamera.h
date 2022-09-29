// Copyright (c) 2022. Johan Lind

#pragma once

#include <glm/glm.hpp>

namespace jle {

    enum class jleCameraProjection {
        Orthographic,
        Perspective
    };

    struct jleCamera {

        explicit jleCamera() = default;

        explicit jleCamera(jleCameraProjection projection);

        jleCameraProjection mProjectionType = jleCameraProjection::Orthographic;

        glm::vec3 mPosition{0.f, 24.f, 0.f};

        glm::mat4 mProjectionMatrix{};

        // For perspective projection
        glm::vec3 mFront{0.0f, 0.0f, -1.0f};
        glm::vec3 mUp{0.0f, 1.0f, 0.0f};
        glm::vec3 mRight{};
        const glm::vec3 mWorldUp{0.0f, 1.0f, 0.0f};
        float mYaw{-90.f};
        float mPitch{0.f};
        float mMouseSensitivity{0.001f};

        // For orthographic projection
        float mCameraRotationDegrees{};

        void SetPerspectiveProjection(float fov, uint32_t screenWidth, uint32_t screenHeight,
                                      float farPlane, float nearPlane);

        void SetOrthographicProjection(uint32_t screenWidth, uint32_t screenHeight, float farPlane, float nearPlane);

        [[nodiscard]] glm::mat4 GetProjectionViewMatrix() const;

        void SetPerspectiveMouseSensitivity(float sensitivity);

        void ApplyPerspectiveMouseMovementDelta(glm::vec2 delta);

        void CalculatePerspectiveVectors();

        void MovePerspectiveForward(float speed);
        void MovePerspectiveBackward(float speed);
        void MovePerspectiveRight(float speed);
        void MovePerspectiveLeft(float speed);
        void MovePerspectiveUp(float speed);
        void MovePerspectiveDown(float speed);

        float mX = 0.f, mY = 0.f;
        float mXNoOffset = 0.f, mYNoOffset = 0.f;

        int mCameraWidth{}, mCameraHeight{};

        [[nodiscard]] int32_t GetIntX() const {
            return int32_t(mX);
        }

        [[nodiscard]] int32_t GetIntY() const {
            return int32_t(mY);
        }
    };
}
