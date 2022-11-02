// Copyright (c) 2022. Johan Lind

#include "oCharacter.h"
#include "hexHelperFunctions.h"
#include "hexHexagonFunctions.h"
#include "jleCore.h"
#include "oWorld.h"

void oCharacter::setupDefaultObject() {
    _transform = addCustomComponent<cTransform>();
    _aseprite = addCustomComponent<cAseprite>();

    _healthBarObjPtr = std::static_pointer_cast<oCharacterHealthBar>(
        spawnChildObjectFromTemplate(
            jleRelativePath{"GR:otemps/oCharacterHealthBar.tmpl"}));
}

void oCharacter::start() {
    const auto &&placement = _hexagonItem.hexagonItemPlacement();
    hexagonPlacementTeleport(placement.x, placement.y);

    _currentHP = _maxHP;

    if (!_showHpBar) {
        _healthBarObjPtr->destroyObject();
    }
}

void oCharacter::update(float dt) {

    if (gCore->input().keyboard->keyPressed('T')) {
        hexagonPlacementInterp(hexHelperFunctions::randInt(0, 10),
                               hexHelperFunctions::randInt(0, 10));
    }
    const auto lerpVec2 =
        [](const glm::vec2 &a, const glm::vec2 &b, float alpha) {
            return a * alpha + b * (1.f - alpha);
        };

    if (_interpingPosition) {

        _aseprite->currentAseprite(_walkAsepriteIndex);

        auto pos = lerpVec2({_hexagonPixelX, _hexagonPixelY},
                            {_interpingX, _interpingY},
                            _interpingAlpha);

        _interpingX = pos.x;
        _interpingY = pos.y;
        _transform->setWorldPosition({(int)_interpingX, (int)_interpingY, 0.f});

        _interpingAlpha += _interpBetweenHexasSpeed * dt;
        if (_interpingAlpha >= 1.f) {
            _interpingAlpha = 1.f;
            _interpingPosition = false;
            _aseprite->currentAseprite(_idleAsepriteIndex);
        }
    }
}

void oCharacter::characterDirection(oCharacterDirection direction) {
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

void oCharacter::hexagonPlacementTeleport(int q, int r) {
    if (!_hexagonItem.tryUpdateHexagonItemPlacement(q, r)) {
        return;
    }

    const auto *world = oWorld::sWorld;
    auto p = hexHexagonFunctions::hexToPixel(
        q, r, world->_hexSizeX, world->_hexSizeY);

    _hexagonPixelX = p.x;
    _hexagonPixelY = p.y;

    _transform->setWorldPosition({_hexagonPixelX, _hexagonPixelY, 0.f});
    _interpingPosition = false;
}

void oCharacter::hexagonPlacementInterp(int q, int r) {
    if (!_hexagonItem.tryUpdateHexagonItemPlacement(q, r)) {
        return;
    }

    const auto *world = oWorld::sWorld;
    auto p = hexHexagonFunctions::hexToPixel(q, r, world->_hexSizeX, world->_hexSizeY);

    _hexagonPixelX = p.x;
    _hexagonPixelY = p.y;

    _interpingX = _transform->getWorldPosition().x;
    _interpingY = _transform->getWorldPosition().y;

    _interpingPosition = true;
    _interpingAlpha = 0.f;
}

void oCharacter::toJson(nlohmann::json &j_out) {
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

void oCharacter::fromJson(const nlohmann::json &j_in) {
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _hexagonItem, "_hexagonItem", {});

    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _interpBetweenHexasSpeed, "_interpBetweenHexasSpeed", 1.f);

    JLE_FROM_JSON_WITH_DEFAULT(j_in,
                               _attackCooldownAfterAnimationMs,
                               "_attackCooldownAfterAnimationMs",
                               0.f);

    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _healthBarObjectTemplatePath, "_healthBarObjectTemplatePath", "");

    JLE_FROM_JSON_WITH_DEFAULT(j_in, _maxHP, "_maxHP", 50);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _showHpBar, "_showHpBar", true);

    JLE_FROM_JSON_WITH_DEFAULT(j_in, _westTextureX, "_westTextureX", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _westTextureY, "_westTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _northwestTextureX, "_northwestTextureX", 32);
    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _northwestTextureY, "_northwestTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _northTextureX, "_northTextureX", 64);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _northTextureY, "_northTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _northeastTextureX, "_northeastTextureX", 96);
    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _northeastTextureY, "_northeastTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _eastTextureX, "_eastTextureX", 128);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _eastTextureY, "_eastTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _southeastTextureX, "_southeastTextureX", 160);
    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _southeastTextureY, "_southeastTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _southTextureX, "_southTextureX", 192);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _southTextureY, "_southTextureY", 0);
    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _southwestTextureX, "_southwestTextureX", 224);
    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _southwestTextureY, "_southwestTextureY", 0);
}

void oCharacter::attack(oCharacter::oCharacterDirection dir) {

    characterDirection(dir);

    if (!_canAttack) {
        return;
    }

    _aseprite->currentAseprite(_attackAsepriteIndex);
    _aseprite->currentAsepriteFrame(0);

    const double animationTimeMs =
        _aseprite->activeAsepriteRef().totalAnimationTimeMs();

    const auto futureFunc = [](std::weak_ptr<void> data) {
        auto safeThis = std::static_pointer_cast<oCharacter>(data.lock());
        safeThis->_aseprite->currentAseprite(0);
        safeThis->_aseprite->currentAsepriteFrame(0);
    };

    // Go back to the default animation
    gCore->timerManager().executeFuncInSecondsWeakData(
        animationTimeMs * 0.001, futureFunc, weakPtrToThis());

    // Can not attack again for the animation time + some additional time
    gCore->timerManager().executeFuncInSecondsWeakData(
        (animationTimeMs + _attackCooldownAfterAnimationMs) * 0.001,
        [](std::weak_ptr<void> data) {
            auto safeThis = std::static_pointer_cast<oCharacter>(data.lock());
            safeThis->_canAttack = true;
        },
        weakPtrToThis());

    _canAttack = false;
}

void oCharacter::hp(int hp) {
    if (hp > _maxHP) {
        hp = _maxHP;
    }

    if (_healthBarObjPtr) {
        _healthBarObjPtr->hp(_maxHP, hp);
    }
}

void oCharacter::lookAtPosition(int x, int y)
{
    constexpr int smallXAdjustment = -2;

    glm::vec2 target = {x, y};
    glm::vec2 origin = {_transform->getWorldPosition().x + smallXAdjustment, _transform->getWorldPosition().y};

    glm::vec2 vector2 = target - origin;
    glm::vec2 vector1{0, 1};

    const double angleRad = atan2(vector2.y, vector2.x) - atan2(vector1.y, vector1.x);
    const double angleDeg = glm::degrees(angleRad);

    if (angleDeg > -240.0 && angleDeg < -180) {
        characterDirection(oCharacterDirection::northwest);
    }

    if (angleDeg > -180 && angleDeg < -120) {
        characterDirection(oCharacterDirection::northeast);
    }

    if (angleDeg > -120 && angleDeg < -60) {
        characterDirection(oCharacterDirection::east);
    }

    if (angleDeg > -60 && angleDeg < 0) {
        characterDirection(oCharacterDirection::southeast);
    }

    if (angleDeg > 0 && angleDeg < 60) {
        characterDirection(oCharacterDirection::southwest);
    }

    if (angleDeg > 60 && angleDeg < 240) {
        characterDirection(oCharacterDirection::west);
    }
}
