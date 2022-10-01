// Copyright (c) 2022. Johan Lind

#include "oCharacter.h"
#include "hexHexagonFunctions.h"
#include "hexHelperFunctions.h"
#include "oWorld.h"
#include "jleCore.h"

void oCharacter::SetupDefaultObject() {
    _transform = AddCustomComponent<cTransform>();
    _aseprite = AddCustomComponent<cAseprite>();

    _healthBarObjPtr = std::static_pointer_cast<oCharacterHealthBar>(
            SpawnChildObjectFromTemplate(jleRelativePath{"GR:otemps/oCharacterHealthBar.tmpl"}));
}

void oCharacter::Start() {
    const auto &&placement = _hexagonItem.GetHexagonItemPlacement();
    SetHexagonPlacementTeleport(placement.x, placement.y);

    _currentHP = _maxHP;

    if (!_showHpBar) {
        _healthBarObjPtr->DestroyObject();
    }
}

void oCharacter::Update(float dt) {

    if (jleCore::core->input->keyboard->GetKeyPressed('T')) {
        SetHexagonPlacementInterp(hexHelperFunctions::GetRandInt(0, 10), hexHelperFunctions::GetRandInt(0, 10));
    }
    const auto lerpVec2 = [](const glm::vec2 &a, const glm::vec2 &b, float alpha) {
        return a * alpha + b * (1.f - alpha);
    };

    if (_interpingPosition) {

        _aseprite->SetCurrentAseprite(_walkAsepriteIndex);

        auto pos = lerpVec2(
                {_hexagonPixelX, _hexagonPixelY},
                {_interpingX, _interpingY}, _interpingAlpha);

        _interpingX = pos.x;
        _interpingY = pos.y;
        _transform->SetWorldPositionXY((int) _interpingX, (int) _interpingY);

        _interpingAlpha += _interpBetweenHexasSpeed * dt;
        if (_interpingAlpha >= 1.f) {
            _interpingAlpha = 1.f;
            _interpingPosition = false;
            _aseprite->SetCurrentAseprite(_idleAsepriteIndex);
        }
    }

}

void oCharacter::SetCharacterDirection(oCharacterDirection direction) {
    _characterDirection = direction;

    switch (direction) {
        case oCharacterDirection::west:
            _aseprite->_textureX = _westTextureX;
            _aseprite->_textureY = _westTextureY;
            break;
        case oCharacterDirection::northwest:
            _aseprite->_textureX = _northwestTextureX;
            _aseprite->_textureY = _northwestTextureY;
            break;
        case oCharacterDirection::north:
            _aseprite->_textureX = _northTextureX;
            _aseprite->_textureY = _northTextureY;
            break;
        case oCharacterDirection::northeast:
            _aseprite->_textureX = _northeastTextureX;
            _aseprite->_textureY = _northeastTextureY;
            break;
        case oCharacterDirection::east:
            _aseprite->_textureX = _eastTextureX;
            _aseprite->_textureY = _eastTextureY;
            break;
        case oCharacterDirection::southeast:
            _aseprite->_textureX = _southeastTextureX;
            _aseprite->_textureY = _southeastTextureY;
            break;
        case oCharacterDirection::south:
            _aseprite->_textureX = _southTextureX;
            _aseprite->_textureY = _southTextureY;
            break;
        case oCharacterDirection::southwest:
            _aseprite->_textureX = _southwestTextureX;
            _aseprite->_textureY = _southwestTextureY;
            break;
    }

}


void oCharacter::SetHexagonPlacementTeleport(int q, int r) {
    if (!_hexagonItem.TryUpdateHexagonItemPlacement(q, r)) {
        return;
    }


    const auto *world = oWorld::sWorld;
    auto p = hexHexagonFunctions::HexToPixel(q, r, world->_hexSizeX, world->_hexSizeY);

    _hexagonPixelX = p.x;
    _hexagonPixelY = p.y;

    _transform->SetWorldPositionXY(_hexagonPixelX, _hexagonPixelY);
    _interpingPosition = false;
}

void oCharacter::SetHexagonPlacementInterp(int q, int r) {
    if (!_hexagonItem.TryUpdateHexagonItemPlacement(q, r)) {
        return;
    }

    const auto *world = oWorld::sWorld;
    auto p = hexHexagonFunctions::HexToPixel(q, r, world->_hexSizeX, world->_hexSizeY);

    _hexagonPixelX = p.x;
    _hexagonPixelY = p.y;

    _interpingX = _transform->GetWorldX();
    _interpingY = _transform->GetWorldY();

    _interpingPosition = true;
    _interpingAlpha = 0.f;
}


void oCharacter::ToJson(nlohmann::json &j_out) {
    j_out["_hexagonItem"] = _hexagonItem;

    j_out["_interpBetweenHexasSpeed"] = _interpBetweenHexasSpeed;

    j_out["_attackCooldownAfterAnimationMs"] = _attackCooldownAfterAnimationMs;

    j_out["_maxHP"] = _maxHP;
    j_out["_showHpBar"] = _showHpBar;

    j_out["_healthBarObjectTemplatePath"] = _healthBarObjectTemplatePath;

    j_out["_westTextureX"] = _westTextureX;
    j_out["_westTextureY"] = _westTextureY;
    j_out["_northwestTextureX"] = _northwestTextureX;
    j_out["_northwestTextureY"] = _northwestTextureY;
    j_out["_northTextureX"] = _northTextureX;
    j_out["_northTextureY"] = _northTextureY;
    j_out["_northeastTextureX"] = _northeastTextureX;
    j_out["_northeastTextureY"] = _northeastTextureY;
    j_out["_eastTextureX"] = _eastTextureX;
    j_out["_eastTextureY"] = _eastTextureY;
    j_out["_southeastTextureX"] = _southeastTextureX;
    j_out["_southeastTextureY"] = _southeastTextureY;
    j_out["_southTextureX"] = _southTextureX;
    j_out["_southTextureY"] = _southTextureY;
    j_out["_southwestTextureX"] = _southwestTextureX;
    j_out["_southwestTextureY"] = _southwestTextureY;
}

void oCharacter::FromJson(const nlohmann::json &j_in) {
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _hexagonItem, "_hexagonItem", {});

    JLE_FROM_JSON_WITH_DEFAULT(j_in, _interpBetweenHexasSpeed, "_interpBetweenHexasSpeed", 1.f);

    JLE_FROM_JSON_WITH_DEFAULT(j_in, _attackCooldownAfterAnimationMs, "_attackCooldownAfterAnimationMs", 0.f);

    JLE_FROM_JSON_WITH_DEFAULT(j_in, _healthBarObjectTemplatePath, "_healthBarObjectTemplatePath", "");

    JLE_FROM_JSON_WITH_DEFAULT(j_in, _maxHP, "_maxHP", 50);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _showHpBar, "_showHpBar", true);

    JLE_FROM_JSON_WITH_DEFAULT(j_in, _westTextureX, "_westTextureX", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _westTextureY, "_westTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _northwestTextureX, "_northwestTextureX", 32);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _northwestTextureY, "_northwestTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _northTextureX, "_northTextureX", 64);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _northTextureY, "_northTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _northeastTextureX, "_northeastTextureX", 96);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _northeastTextureY, "_northeastTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _eastTextureX, "_eastTextureX", 128);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _eastTextureY, "_eastTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _southeastTextureX, "_southeastTextureX", 160);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _southeastTextureY, "_southeastTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _southTextureX, "_southTextureX", 192);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _southTextureY, "_southTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _southwestTextureX, "_southwestTextureX", 224);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _southwestTextureY, "_southwestTextureY", 0);
}

void oCharacter::Attack(oCharacter::oCharacterDirection dir) {

    SetCharacterDirection(dir);

    if (!_canAttack) {
        return;
    }

    _aseprite->SetCurrentAseprite(_attackAsepriteIndex);
    _aseprite->SetCurrentAsepriteFrame(0);

    const double animationTimeMs = _aseprite->GetActiveAsepriteRef().GetTotalAnimationTimeMs();

    const auto futureFunc = [](std::weak_ptr<void> data) {
        auto safeThis = std::static_pointer_cast<oCharacter>(data.lock());
        safeThis->_aseprite->SetCurrentAseprite(0);
        safeThis->_aseprite->SetCurrentAsepriteFrame(0);
    };

    // Go back to the default animation
    jleCore::core->GetTimerManager().
            ExecuteFuncInSecondsWeakData(animationTimeMs * 0.001, futureFunc, GetWeakPtrToThis());

    // Can not attack again for the animation time + some additional time
    jleCore::core->GetTimerManager().
            ExecuteFuncInSecondsWeakData(
            (animationTimeMs + _attackCooldownAfterAnimationMs) * 0.001, [](std::weak_ptr<void> data) {
                auto safeThis = std::static_pointer_cast<oCharacter>(data.lock());
                safeThis->_canAttack = true;
            }, GetWeakPtrToThis());

    _canAttack = false;

}

void oCharacter::SetHP(int hp) {
    if (hp > _maxHP) {
        hp = _maxHP;
    }

    if (_healthBarObjPtr) {
        _healthBarObjPtr->SetHP(_maxHP, hp);
    }
}

void oCharacter::LookAtPosition(int x, int y) {
    constexpr int smallXAdjustment = -2;

    glm::vec2 target = {x, y};
    glm::vec2 origin = {_transform->GetWorldX() + smallXAdjustment, _transform->GetWorldY()};

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
