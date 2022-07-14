// Copyright (c) 2022. Johan Lind

#include "cCamera.h"
#include "jleObject.h"
#include "jleGameEngine.h"

void jle::cCamera::Start() {
    mTransform = mAttachedToObject->AddDependencyComponent<cTransform>(this);
    sInstanceCounter++;

    if (sInstanceCounter > 1) {
        LOG_ERROR << "More than one camera detected!";
    }
}

void jle::cCamera::Update(float dt) {
    auto &game = ((jleGameEngine *) jleCore::core)->GetGameRef();

    game.mMainCamera.mX = mTransform->GetWorldX() + mOffsetX;
    game.mMainCamera.mY = mTransform->GetWorldY() + mOffsetY;

    game.mMainCamera.mXNoOffset = mTransform->GetWorldX();
    game.mMainCamera.mYNoOffset = mTransform->GetWorldY();
}

void jle::cCamera::ToJson(nlohmann::json &j_out) {
    j_out = nlohmann::json{
            {"offsetX", mOffsetX},
            {"offsetY", mOffsetY},
    };
}

void jle::cCamera::FromJson(const nlohmann::json &j_in) {
    mOffsetX = j_in.at("offsetX");
    mOffsetY = j_in.at("offsetY");
}

jle::cCamera::cCamera(jle::jleObject *owner, jle::jleScene *scene) : jleComponent(owner, scene) {}

jle::cCamera::~cCamera() {
    sInstanceCounter--;
}

float jle::cCamera::GetOffsetX() {
    return mOffsetX;
}

float jle::cCamera::GetOffsetY() {
    return mOffsetY;
}
