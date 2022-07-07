// Copyright (c) 2022. Johan Lind

#include "oCharacter.h"
#include "hexHexagonFunctions.h"
#include "oWorld.h"

void oCharacter::SetupDefaultObject() {
    mTransform = AddCustomComponent<cTransform>();
    mAseprite = AddCustomComponent<jle::cAseprite>();
}

void oCharacter::Start() {
    const auto &&placement = GetHexagonItemPlacement();
    SetHexagonPlacementTeleport(placement.x, placement.y);
}

void oCharacter::Update(float dt) {

    const auto lerpVec2 = [](const glm::vec2 &a, const glm::vec2 &b, float alpha) {
        return a * alpha + b * (1.f - alpha);
    };

    if (mInterpingPosition) {
        auto pos = lerpVec2(
                {mHexagonPixelX, mHexagonPixelY},
                {mInterpingX, mInterpingY}, mInterpingAlpha);

        mInterpingX = pos.x;
        mInterpingY = pos.y;
        mTransform->SetWorldPosition((int) mInterpingX, (int) mInterpingY);

        mInterpingAlpha += dt;
        if (mInterpingAlpha >= 1.f) {
            mInterpingAlpha = 1.f;
            mInterpingPosition = false;
        }
    }

}

void oCharacter::SetCharacterDirection(oCharacterDirection direction) {
    mCharacterDirection = direction;

    switch (direction) {
        case oCharacterDirection::west:
            mAseprite->mTextureX = mWestTextureX;
            mAseprite->mTextureY = mWestTextureY;
            break;
        case oCharacterDirection::northwest:
            mAseprite->mTextureX = mNorthwestTextureX;
            mAseprite->mTextureY = mNorthwestTextureY;
            break;
        case oCharacterDirection::north:
            mAseprite->mTextureX = mNorthTextureX;
            mAseprite->mTextureY = mNorthTextureY;
            break;
        case oCharacterDirection::northeast:
            mAseprite->mTextureX = mNortheastTextureX;
            mAseprite->mTextureY = mNortheastTextureY;
            break;
        case oCharacterDirection::east:
            mAseprite->mTextureX = mEastTextureX;
            mAseprite->mTextureY = mEastTextureY;
            break;
        case oCharacterDirection::southeast:
            mAseprite->mTextureX = mSoutheastTextureX;
            mAseprite->mTextureY = mSoutheastTextureY;
            break;
        case oCharacterDirection::south:
            mAseprite->mTextureX = mSouthTextureX;
            mAseprite->mTextureY = mSouthTextureY;
            break;
        case oCharacterDirection::southwest:
            mAseprite->mTextureX = mSouthwestTextureX;
            mAseprite->mTextureY = mSouthwestTextureY;
            break;
    }

}


void oCharacter::SetHexagonPlacementTeleport(int q, int r) {
    if (!TryUpdateHexagonItemPlacement(q, r)) {
        return;
    }


    const auto *world = oWorld::sWorld;
    auto p = hexHexagonFunctions::HexToPixel(q, r, world->mHexSizeX, world->mHexSizeY);

    mHexagonPixelX = p.x;
    mHexagonPixelY = p.y;

    mTransform->SetWorldPosition(mHexagonPixelX, mHexagonPixelY);
    mInterpingPosition = false;
}

void oCharacter::SetHexagonPlacementInterp(int q, int r) {
    if (!TryUpdateHexagonItemPlacement(q, r)) {
        return;
    }

    const auto *world = oWorld::sWorld;
    auto p = hexHexagonFunctions::HexToPixel(q, r, world->mHexSizeX, world->mHexSizeY);

    mHexagonPixelX = p.x;
    mHexagonPixelY = p.y;

    mInterpingX = mTransform->GetWorldX();
    mInterpingY = mTransform->GetWorldY();

    mInterpingPosition = true;
    mInterpingAlpha = 0.f;
}


void oCharacter::ToJson(nlohmann::json &j_out) {
    const auto &&hexagonPlacement = GetHexagonItemPlacement();
    j_out["hexagonPlacementQ"] = hexagonPlacement.x;
    j_out["hexagonPlacementR"] = hexagonPlacement.y;
    j_out["mWestTextureX"] = mWestTextureX;
    j_out["mWestTextureY"] = mWestTextureY;
    j_out["mNorthwestTextureX"] = mNorthwestTextureX;
    j_out["mNorthwestTextureY"] = mNorthwestTextureY;
    j_out["mNorthTextureX"] = mNorthTextureX;
    j_out["mNorthTextureY"] = mNorthTextureY;
    j_out["mNortheastTextureX"] = mNortheastTextureX;
    j_out["mNortheastTextureY"] = mNortheastTextureY;
    j_out["mEastTextureX"] = mEastTextureX;
    j_out["mEastTextureY"] = mEastTextureY;
    j_out["mSoutheastTextureX"] = mSoutheastTextureX;
    j_out["mSoutheastTextureY"] = mSoutheastTextureY;
    j_out["mSouthTextureX"] = mSouthTextureX;
    j_out["mSouthTextureY"] = mSouthTextureY;
    j_out["mSouthwestTextureX"] = mSouthwestTextureX;
    j_out["mSouthwestTextureY"] = mSouthwestTextureY;
}

void oCharacter::FromJson(const nlohmann::json &j_in) {
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mWestTextureX, "mWestTextureX", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mWestTextureY, "mWestTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mNorthwestTextureX, "mNorthwestTextureX", 32);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mNorthwestTextureY, "mNorthwestTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mNorthTextureX, "mNorthTextureX", 64);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mNorthTextureY, "mNorthTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mNortheastTextureX, "mNortheastTextureX", 96);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mNortheastTextureY, "mNortheastTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mEastTextureX, "mEastTextureX", 128);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mEastTextureY, "mEastTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mSoutheastTextureX, "mSoutheastTextureX", 160);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mSoutheastTextureY, "mSoutheastTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mSouthTextureX, "mSouthTextureX", 192);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mSouthTextureY, "mSouthTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mSouthwestTextureX, "mSouthwestTextureX", 224);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mSouthwestTextureY, "mSouthwestTextureY", 0);

    int hexagonPlacementQ = 0, hexagonPlacementR = 0;
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mWestTextureX, "hexagonPlacementQ", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mWestTextureX, "hexagonPlacementR", 0);
    SetHexagonItemPlacement(hexagonPlacementQ, hexagonPlacementR);
}
