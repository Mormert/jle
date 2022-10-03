// Copyright (c) 2022. Johan Lind

#pragma once

#include "cTransform.h"
#include "jleComponent.h"
#include "jleSpritesheet.h"

class cSpritesheet : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cSpritesheet)
public:
    explicit cSpritesheet(jleObject *owner = nullptr,
                          jleScene *scene = nullptr);

    void Start() override;

    void Update(float dt) override;

    void ToJson(nlohmann::json& j_out) override;

    void FromJson(const nlohmann::json& j_in) override;

private:
    std::shared_ptr<cTransform> _transform{nullptr};
    std::shared_ptr<jleSpritesheet> _spritesheet;

    bool _hasEntity = false;
    jleSpritesheetEntity _spritesheetEntityCache;

    std::string _spritesheetPath;
    std::string _spriteName;
};
