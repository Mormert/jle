// Copyright (c) 2022. Johan Lind

#include "cCamera.h"
#include "jleGameEngine.h"
#include "jleObject.h"

void cCamera::Start() {
    _transform = _attachedToObject->AddDependencyComponent<cTransform>(this);
    sInstanceCounter++;

    if (sInstanceCounter > 1) {
        LOG_ERROR << "More than one camera detected!";
    }
}

void cCamera::Update(float dt) {
    auto& game = ((jleGameEngine *)jleCore::core)->GetGameRef();

    game._mainCamera._x = _transform->GetWorldX() + _offsetX;
    game._mainCamera._y = _transform->GetWorldY() + _offsetY;

    game._mainCamera._xNoOffset = _transform->GetWorldX();
    game._mainCamera._yNoOffset = _transform->GetWorldY();
}

void cCamera::ToJson(nlohmann::json& j_out) {
    j_out = nlohmann::json{
        {"offsetX", _offsetX},
        {"offsetY", _offsetY},
    };
}

void cCamera::FromJson(const nlohmann::json& j_in) {
    _offsetX = j_in.at("offsetX");
    _offsetY = j_in.at("offsetY");
}

cCamera::cCamera(jleObject *owner, jleScene *scene)
    : jleComponent(owner, scene) {}

cCamera::~cCamera() { sInstanceCounter--; }

float cCamera::GetOffsetX() { return _offsetX; }

float cCamera::GetOffsetY() { return _offsetY; }
