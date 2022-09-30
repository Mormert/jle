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

    void Start() override;

    void ToJson(nlohmann::json& j_out) override;

    void FromJson(const nlohmann::json& j_in) override;

    void Update(float dt) override;

    void SetText(const std::string& text);

private:
    std::shared_ptr<cTransform> mTransform{nullptr};
    std::shared_ptr<jleFont> mFont{nullptr};

    std::string mFontPath;
    std::string mText;
    uint32_t mFontSize{16};
    float mColorR{1.f}, mColorG{1.f}, mColorB{1.f}, mColorA{1.f};
};
