// Copyright (c) 2022. Johan Lind

#ifndef HEXABLO_OCHARACTER_H
#define HEXABLO_OCHARACTER_H

#include "jleObject.h"
#include "cTransform.h"
#include "cSprite.h"
#include "cAseprite.h"
#include "oCharacterHealthBar.h"
#include "hexHexagonItem.h"

#include <memory>
#include <iostream>

class oCharacter : public jleObject {
    JLE_REGISTER_OBJECT_TYPE(oCharacter)
public:

    enum class oCharacterDirection {
        west,
        northwest,
        north,
        northeast,
        east,
        southeast,
        south,
        southwest
    };

    virtual void Attack(oCharacterDirection direction);

    void SetHP(int hp);

    void SetCharacterDirection(oCharacterDirection direction);

    void SetHexagonPlacementTeleport(int q, int r);

    void SetHexagonPlacementInterp(int q, int r);

    void SetupDefaultObject() override;

    void Start() override;

    void Update(float dt) override;

    void ToJson(nlohmann::json &j_out) override;

    void FromJson(const nlohmann::json &j_in) override;

    std::shared_ptr<cTransform> _transform{nullptr};
    std::shared_ptr<cAseprite> _aseprite{nullptr};

protected:

    void LookAtPosition(int x, int y);

    hexHexagonItem _hexagonItem;

    int _hexagonPixelX{}, _hexagonPixelY{};

    float _interpingX{}, _interpingY{};
    float _interpingAlpha{0.f};
    bool _interpingPosition{false};
    float _interpBetweenHexasSpeed = 1.f;

    oCharacterDirection _characterDirection{};

    unsigned int _idleAsepriteIndex{0};
    unsigned int _attackAsepriteIndex{1};
    unsigned int _walkAsepriteIndex{2};

    float _attackCooldownAfterAnimationMs{0};

    bool _showHpBar = true;
    std::shared_ptr<oCharacterHealthBar> _healthBarObjPtr{};

    int _maxHP{};
    int _currentHP{};

    std::string _healthBarObjectTemplatePath;

    bool _canAttack = true;

    int _westTextureX{};
    int _westTextureY{};

    int _northwestTextureX{32};
    int _northwestTextureY{};

    int _northTextureX{64};
    int _northTextureY{};

    int _northeastTextureX{96};
    int _northeastTextureY{};

    int _eastTextureX{128};
    int _eastTextureY{};

    int _southeastTextureX{160};
    int _southeastTextureY{};

    int _southTextureX{192};
    int _southTextureY{};

    int _southwestTextureX{224};
    int _southwestTextureY{};

};


#endif //HEXABLO_OCHARACTER_H
