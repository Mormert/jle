/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "cCameraFPV.h"
#include "glm/ext/matrix_transform.hpp"
#include "jleGameEngine.h"
#include "jleInput.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cCameraFPV)

cCameraFPV::~cCameraFPV() {}

void
cCameraFPV::update(float dt)
{
    auto &&keyboard = gEngine->input().keyboard;

    auto updatedPosition = getTransform().getLocalPosition();

    auto right = getTransform().getRight();
    auto forward = getTransform().getForward();
    auto up = getTransform().getUp();

    glm::vec3 addedPosition{0.f};

    if (keyboard->keyDown(jleKey::A)) {
        addedPosition -= right;
    }
    if (keyboard->keyDown(jleKey::D)) {
        addedPosition += right;
    }
    if (keyboard->keyDown(jleKey::W)) {
        addedPosition -= forward;
    }
    if (keyboard->keyDown(jleKey::S)) {
        addedPosition += forward;
    }
    if (keyboard->keyDown(jleKey::SPACE)) {
        addedPosition -= up;
    }
    if (keyboard->keyDown(jleKey::LEFT_CONTROL)) {
        addedPosition += up;
    }

    if (addedPosition != glm::vec3{0.f}) {
        addedPosition = glm::normalize(addedPosition) * dt * _moveSpeed;
        if (keyboard->keyDown(jleKey::LEFT_SHIFT)) {
            addedPosition *= 2.5f;
        }
        updatedPosition += addedPosition;

        getTransform().setLocalPosition(updatedPosition);
    }

    JLE_EXEC_IF_NOT(JLE_BUILD_EDITOR)
    {
        if (keyboard->keyPressed(jleKey::TAB)) // Tab
        {
            auto fpsMode = gEngine->input().mouse->isFpsMode();
            if (fpsMode) {
                gEngine->input().mouse->setFpsMode(false);
            } else {
                gEngine->input().mouse->setFpsMode(true);
            }
        }
    }

    float mouseDeltaX = gEngine->input().mouse->xDelta() * 0.078f;
    float mouseDeltaY = -gEngine->input().mouse->yDelta() * 0.078f;

    glm::mat4 worldMatrix = getTransform().getWorldMatrix();

    const auto rotateViewByMouseInput = [&](glm::mat4 &viewMatrix) {
        glm::vec3 direction = glm::normalize(glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]));

        // Calculate yaw (rotation around y-axis)
        float yaw = atan2(direction.z, direction.x);
        yaw = glm::degrees(yaw);

        // Calculate pitch (rotation around x-axis)
        float pitch = asin(-direction.y);
        pitch = glm::degrees(pitch);

        yaw += mouseDeltaX;
        pitch -= mouseDeltaY;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        glm::vec3 position = worldMatrix[3];

        viewMatrix = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
    };

    auto viewMatrix = glm::inverse(worldMatrix);

    // Ok I have no idea but for some reason, there's a flickering due to the direction vector going from front to back
    // each time the rotateViewByMouseInput lambda is called, thus, if we call it twice,
    // we mitigate this odd behaviour. Fight me IRL if you don't agree on this solution :)
    // Also: We do this whole pitch/yaw calculation to avoid having to keep internal variables for those, and
    // we can fetch them directly from the world matrix, which means that you can rotate the camera externally.
    rotateViewByMouseInput(viewMatrix);
    rotateViewByMouseInput(viewMatrix);

    getTransform().setWorldMatrix(glm::inverse(viewMatrix));
}
