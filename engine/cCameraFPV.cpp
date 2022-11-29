// Copyright (c) 2022. Johan Lind

#include "cCameraFPV.h"
#include "jleGameEngine.h"

cCameraFPV::cCameraFPV(jleObject *owner, jleScene *scene) : cCamera(owner, scene) {}

cCameraFPV::~cCameraFPV() {}

void
cCameraFPV::toJson(nlohmann::json &j_out)
{
    cCamera::toJson(j_out);
    j_out["moveSpeed"] = _moveSpeed;
    j_out["mouseSensitivity"] = _mouseSensitivity;
}

void
cCameraFPV::fromJson(const nlohmann::json &j_in)
{
    cCamera::fromJson(j_in);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _moveSpeed, "moveSpeed", 10.f);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _mouseSensitivity, "mouseSensitivity", 1.f);
}

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

    const auto mouseDeltaX = gCore->input().mouse->xDelta();
    const auto mouseDeltaY = gCore->input().mouse->yDelta();

    _fpvController.setPerspectiveMouseSensitivity(_mouseSensitivity);
    _fpvController.applyPerspectiveMouseMovementDelta({mouseDeltaX, mouseDeltaY}, dt);

    game.mainCamera.setViewMatrix(_fpvController.getLookAtViewMatrix(), _fpvController.position);
}
