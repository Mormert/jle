// Copyright (c) 2022. Johan Lind

#pragma once

#include "cTransform.h"
#include "jleAseprite.h"
#include "jleComponent.h"
#include "jleFont.h"

class cText : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cText)
public:
    explicit cText(jleObject *owner = nullptr, jleScene *scene = nullptr);

    void start() override;

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

    void update(float dt) override;

    void text(const std::string &text);

private:
    std::shared_ptr<cTransform> _transform{nullptr};
    std::shared_ptr<jleFont> _font{nullptr};

    std::string _fontPath;
    std::string _text;
    uint32_t _fontSize{16};
    float _colorR{1.f}, _colorG{1.f}, _colorB{1.f}, _colorA{1.f};
};
