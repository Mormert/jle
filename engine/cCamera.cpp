// Copyright (c) 2022. Johan Lind

#include "cCamera.h"
#include "jleGameEngine.h"
#include "jleObject.h"

void cCamera::start() {
    _transform = _attachedToObject->addDependencyComponent<cTransform>(this);
    sInstanceCounter++;

    if (sInstanceCounter > 1) {
        LOG_ERROR << "More than one camera detected!";
    }
}

void cCamera::update(float dt) {
    auto &game = ((jleGameEngine *)gCore)->gameRef();

    game._mainCamera._x = _transform->worldPosition().x + _offsetX;
    game._mainCamera._y = _transform->worldPosition().y + _offsetY;

    game._mainCamera._xNoOffset = _transform->worldPosition().x;
    game._mainCamera._yNoOffset = _transform->worldPosition().y;
}

void cCamera::toJson(nlohmann::json &j_out) {
    j_out = nlohmann::json{
        {"offsetX", _offsetX},
        {"offsetY", _offsetY},
    };
}

void cCamera::fromJson(const nlohmann::json &j_in) {
    _offsetX = j_in.at("offsetX");
    _offsetY = j_in.at("offsetY");
}

cCamera::cCamera(jleObject *owner, jleScene *scene)
    : jleComponent(owner, scene) {}

cCamera::~cCamera() { sInstanceCounter--; }

float cCamera::offsetX() const { return _offsetX; }

float cCamera::offsetY() const { return _offsetY; }
