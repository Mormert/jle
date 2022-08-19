// Copyright (c) 2022. Johan Lind

#include "oCharacter.h"
#include "hexHexagonFunctions.h"
#include "hexHelperFunctions.h"
#include "oWorld.h"
#include "jleCore.h"

void oCharacter::SetupDefaultObject() {
    mTransform = AddCustomComponent<cTransform>();
    mAseprite = AddCustomComponent<jle::cAseprite>();

    mHealthBarObjPtr = std::static_pointer_cast<oCharacterHealthBar>(
            SpawnChildObjectFromTemplate(jleRelativePath{"GR:otemps/oCharacterHealthBar.tmpl"}));
}

void oCharacter::Start() {
    const auto &&placement = mHexagonItem.GetHexagonItemPlacement();
    SetHexagonPlacementTeleport(placement.x, placement.y);

    mCurrentHP = mMaxHP;

    if (!mShowHpBar) {
        mHealthBarObjPtr->DestroyObject();
    }
}

void oCharacter::Update(float dt) {

    if (jle::jleCore::core->input->keyboard->GetKeyPressed('T')) {
        SetHexagonPlacementInterp(hexHelperFunctions::GetRandInt(0, 10), hexHelperFunctions::GetRandInt(0, 10));
    }
    const auto lerpVec2 = [](const glm::vec2 &a, const glm::vec2 &b, float alpha) {
        return a * alpha + b * (1.f - alpha);
    };

    if (mInterpingPosition) {

        mAseprite->SetCurrentAseprite(mWalkAsepriteIndex);

        auto pos = lerpVec2(
                {mHexagonPixelX, mHexagonPixelY},
                {mInterpingX, mInterpingY}, mInterpingAlpha);

        mInterpingX = pos.x;
        mInterpingY = pos.y;
        mTransform->SetWorldPositionXY((int) mInterpingX, (int) mInterpingY);

        mInterpingAlpha += mInterpBetweenHexasSpeed * dt;
        if (mInterpingAlpha >= 1.f) {
            mInterpingAlpha = 1.f;
            mInterpingPosition = false;
            mAseprite->SetCurrentAseprite(mIdleAsepriteIndex);
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
    if (!mHexagonItem.TryUpdateHexagonItemPlacement(q, r)) {
        return;
    }


    const auto *world = oWorld::sWorld;
    auto p = hexHexagonFunctions::HexToPixel(q, r, world->mHexSizeX, world->mHexSizeY);

    mHexagonPixelX = p.x;
    mHexagonPixelY = p.y;

    mTransform->SetWorldPositionXY(mHexagonPixelX, mHexagonPixelY);
    mInterpingPosition = false;
}

void oCharacter::SetHexagonPlacementInterp(int q, int r) {
    if (!mHexagonItem.TryUpdateHexagonItemPlacement(q, r)) {
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
    j_out["mHexagonItem"] = mHexagonItem;

    j_out["mInterpBetweenHexasSpeed"] = mInterpBetweenHexasSpeed;

    j_out["mAttackCooldownAfterAnimationMs"] = mAttackCooldownAfterAnimationMs;

    j_out["mMaxHP"] = mMaxHP;
    j_out["mShowHpBar"] = mShowHpBar;

    j_out["mHealthBarObjectTemplatePath"] = mHealthBarObjectTemplatePath;

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
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mHexagonItem, "mHexagonItem", {});

    JLE_FROM_JSON_WITH_DEFAULT(j_in, mInterpBetweenHexasSpeed, "mInterpBetweenHexasSpeed", 1.f);

    JLE_FROM_JSON_WITH_DEFAULT(j_in, mAttackCooldownAfterAnimationMs, "mAttackCooldownAfterAnimationMs", 0.f);

    JLE_FROM_JSON_WITH_DEFAULT(j_in, mHealthBarObjectTemplatePath, "mHealthBarObjectTemplatePath", "");

    JLE_FROM_JSON_WITH_DEFAULT(j_in, mMaxHP, "mMaxHP", 50);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mShowHpBar, "mShowHpBar", true);

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
}

void oCharacter::Attack(oCharacter::oCharacterDirection dir) {

    SetCharacterDirection(dir);

    if (!mCanAttack) {
        return;
    }

    mAseprite->SetCurrentAseprite(mAttackAsepriteIndex);
    mAseprite->SetCurrentAsepriteFrame(0);

    const double animationTimeMs = mAseprite->GetActiveAsepriteRef().GetTotalAnimationTimeMs();

    const auto futureFunc = [](std::weak_ptr<void> data) {
        auto safeThis = std::static_pointer_cast<oCharacter>(data.lock());
        safeThis->mAseprite->SetCurrentAseprite(0);
        safeThis->mAseprite->SetCurrentAsepriteFrame(0);
    };

    // Go back to the default animation
    jle::jleCore::core->GetTimerManager().
            ExecuteFuncInSecondsWeakData(animationTimeMs * 0.001, futureFunc, GetWeakPtrToThis());

    // Can not attack again for the animation time + some additional time
    jle::jleCore::core->GetTimerManager().
            ExecuteFuncInSecondsWeakData(
            (animationTimeMs + mAttackCooldownAfterAnimationMs) * 0.001, [](std::weak_ptr<void> data) {
                auto safeThis = std::static_pointer_cast<oCharacter>(data.lock());
                safeThis->mCanAttack = true;
            }, GetWeakPtrToThis());

    mCanAttack = false;

}

void oCharacter::SetHP(int hp) {
    if (hp > mMaxHP) {
        hp = mMaxHP;
    }

    if (mHealthBarObjPtr) {
        mHealthBarObjPtr->SetHP(mMaxHP, hp);
    }
}

void oCharacter::LookAtPosition(int x, int y) {
    constexpr int smallXAdjustment = -2;

    glm::vec2 target = {x, y};
    glm::vec2 origin = {mTransform->GetWorldX() + smallXAdjustment, mTransform->GetWorldY()};

    glm::vec2 vector2 = target - origin;
    glm::vec2 vector1{0, 1};

    const double angleRad = atan2(vector2.y, vector2.x) - atan2(vector1.y, vector1.x);
    const double angleDeg = glm::degrees(angleRad);

    if (angleDeg > -240.0 && angleDeg < -180) {
        SetCharacterDirection(oCharacterDirection::northwest);
    }

    if (angleDeg > -180 && angleDeg < -120) {
        SetCharacterDirection(oCharacterDirection::northeast);
    }

    if (angleDeg > -120 && angleDeg < -60) {
        SetCharacterDirection(oCharacterDirection::east);
    }

    if (angleDeg > -60 && angleDeg < 0) {
        SetCharacterDirection(oCharacterDirection::southeast);
    }

    if (angleDeg > 0 && angleDeg < 60) {
        SetCharacterDirection(oCharacterDirection::southwest);
    }

    if (angleDeg > 60 && angleDeg < 240) {
        SetCharacterDirection(oCharacterDirection::west);
    }
}
