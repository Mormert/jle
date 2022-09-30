// Copyright (c) 2022. Johan Lind

#include "oFireball.h"
#include "hexHexagonFunctions.h"
#include "oWorld.h"
#include "jleNetworking.h"
#include <glm/glm.hpp>

void oFireball::SetupDefaultObject() {
    mTransform = AddCustomComponent<cTransform>();
    mAseprite = AddCustomComponent<cAseprite>();
}

void oFireball::Start() {

}

void oFireball::Update(float dt) {

    const auto lerpVec2 = [](const glm::vec2 &a, const glm::vec2 &b, float alpha) {
        return a * alpha + b * (1.f - alpha);
    };

    if (mInterpingPosition) {

        auto pos = lerpVec2(
                {mHexagonPixelX, mHexagonPixelY},
                {mInterpingX, mInterpingY}, mInterpingAlpha);

        mTransform->SetWorldPositionXY((int) pos.x, (int) pos.y);

        mInterpingAlpha += mInterpBetweenHexasSpeed * dt;
        if (mInterpingAlpha >= 1.f) {
            mInterpingAlpha = 1.f;
            mInterpingPosition = false;
            DestroyObject();
        }
    }

}

void oFireball::ToJson(nlohmann::json &j_out) {
    j_out["mMovingTowardsR"] = mMovingTowardsR;
    j_out["mMovingTowardsQ"] = mMovingTowardsQ;
}

void oFireball::FromJson(const nlohmann::json &j_in) {
    JLE_FROM_JSON_IF_EXISTS(j_in, mMovingTowardsR, "mMovingTowardsR")
    JLE_FROM_JSON_IF_EXISTS(j_in, mMovingTowardsQ, "mMovingTowardsQ")
}

void oFireball::FromNet(const nlohmann::json &j_in) {

    JLE_FROM_JSON_IF_EXISTS(j_in, mMovingTowardsR, "r")
    JLE_FROM_JSON_IF_EXISTS(j_in, mMovingTowardsQ, "q")

    int x, y;
    JLE_FROM_JSON_WITH_DEFAULT(j_in, x, "x", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, y, "y", 0);

    mTransform->SetWorldPositionXY(x,y);

    const auto *world = oWorld::sWorld;
    auto p = hexHexagonFunctions::HexToPixel(mMovingTowardsQ, mMovingTowardsR, world->mHexSizeX, world->mHexSizeY);

    mHexagonPixelX = p.x;
    mHexagonPixelY = p.y;

    mInterpingX = mTransform->GetWorldX();
    mInterpingY = mTransform->GetWorldY();

    mInterpingPosition = true;
    mInterpingAlpha = 0.f;

}

void oFireball::ToNet(nlohmann::json &j_out) {
    j_out["r"] = mMovingTowardsR;
    j_out["q"] = mMovingTowardsQ;
    j_out["x"] = mTransform->GetWorldX();
    j_out["y"] = mTransform->GetWorldY();
    j_out["id"] = mId;
}

void oFireball::SetTarget(int q, int r) {
    mMovingTowardsR = r;
    mMovingTowardsQ = q;

    const auto *world = oWorld::sWorld;
    auto p = hexHexagonFunctions::HexToPixel(mMovingTowardsQ, mMovingTowardsR, world->mHexSizeX, world->mHexSizeY);

    mHexagonPixelX = p.x;
    mHexagonPixelY = p.y;

    mInterpingX = mTransform->GetWorldX();
    mInterpingY = mTransform->GetWorldY();

    mInterpingPosition = true;
    mInterpingAlpha = 0.f;

    nlohmann::json j;
    ToNet(j);
    jleNetworking::TryEmitJsonData("oFireball", j);
}

oFireball::oFireball() {
    mId = std::rand();
}
