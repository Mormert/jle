// Copyright (c) 2022. Johan Lind

#pragma once

#include "cAseprite.h"
#include "cTransform.h"
#include "jleObject.h"

class oCharacterHealthBar : public jleObject {
    JLE_REGISTER_OBJECT_TYPE(oCharacterHealthBar)
public:
    void upDefaultObject() override;

    void hp(int maxHP, int currentHP);

    void toJson(nlohmann::json& j_out) override {
        j_out["_maxWidth"] = _maxWidth;
    }

    void fromJson(const nlohmann::json& j_in) override{
        JLE_FROM_JSON_WITH_DEFAULT(j_in, _maxWidth, "_maxWidth", 16)}

    std::shared_ptr<cTransform> _transform;
    std::shared_ptr<cAseprite> _aseprite;

private:
    int _maxWidth{};
};
