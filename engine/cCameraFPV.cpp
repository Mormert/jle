// Copyright (c) 2023. Johan Lind

#include "cCameraFPV.h"
#include "jleGameEngine.h"
#include "jleInput.h"
#include "jleIncludeGL.h"

cCameraFPV::cCameraFPV(jleObject *owner, jleScene *scene) : cCamera(owner, scene) {}

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cCameraFPV)

cCameraFPV::~cCameraFPV() {}

void
cCameraFPV::update(float dt)
{
    auto &game = gEngine->gameRef();

    if (_perspective) {
        game.mainCamera.setPerspectiveProjection(_perspectiveFov,
                                                 gEngine->mainScreenFramebuffer->width(),
                                                 gEngine->mainScreenFramebuffer->height(),
                                                 _farPlane,
                                                 _nearPlane);
    } else {
        game.mainCamera.setOrthographicProjection(
            gEngine->mainScreenFramebuffer->width(), gEngine->mainScreenFramebuffer->height(), _farPlane, _nearPlane);
    }

    const auto &keyboard = gCore->input().keyboard;

    if (keyboard->keyDown('W')) {
        _fpvController.moveForward(dt * _moveSpeed);
    }

    if (keyboard->keyDown('S')) {
        _fpvController.moveBackward(dt * _moveSpeed);
    }

    if (keyboard->keyDown('A')) {
        _fpvController.moveLeft(dt * _moveSpeed);
    }

    if (keyboard->keyDown('D')) {
        _fpvController.moveRight(dt * _moveSpeed);
    }

//#ifndef BUILD_EDITOR
    if (keyboard->keyPressed(258)) // Tab
    {
        auto fpsMode = gEngine->input().mouse->isFpsMode();
        if (fpsMode) {
            gEngine->input().mouse->setFpsMode(false);
        } else {
            gEngine->input().mouse->setFpsMode(true);
        }
    }
//#endif

    const auto mouseDeltaX = gCore->input().mouse->xDelta();
    const auto mouseDeltaY = gCore->input().mouse->yDelta();

    _fpvController.setPerspectiveMouseSensitivity(_mouseSensitivity);
    _fpvController.applyPerspectiveMouseMovementDelta({mouseDeltaX, mouseDeltaY}, dt);

    game.mainCamera.setViewMatrix(_fpvController.getLookAtViewMatrix(), _fpvController.position);
}
