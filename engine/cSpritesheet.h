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

    void start() override;

    void update(float dt) override;

    void toJson(nlohmann::json& j_out) override;

    void fromJson(const nlohmann::json& j_in) override;

    void entity(const std::string& entityName);

private:
    std::shared_ptr<cTransform> _transform{nullptr};
    std::shared_ptr<jleSpritesheet> _spritesheet;

    bool _hasEntity = false;
    jleSpritesheetEntity _spritesheetEntityCache;

    std::string _spritesheetPath;
    std::string _spriteName;
    glm::vec2 _offset{};
};
