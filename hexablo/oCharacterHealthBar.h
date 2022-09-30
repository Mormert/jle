// Copyright (c) 2022. Johan Lind

#ifndef HEXABLO_OCHARACTERHEALTHBAR_H
#define HEXABLO_OCHARACTERHEALTHBAR_H

#include "cAseprite.h"
#include "cTransform.h"
#include "jleObject.h"

class oCharacterHealthBar : public jleObject {
    JLE_REGISTER_OBJECT_TYPE(oCharacterHealthBar)
public:
    void SetupDefaultObject() override;

    void SetHP(int maxHP, int currentHP);

    void ToJson(nlohmann::json& j_out) override {
        j_out["mMaxWidth"] = mMaxWidth;
    }

    void FromJson(const nlohmann::json& j_in) override{
        JLE_FROM_JSON_WITH_DEFAULT(j_in, mMaxWidth, "mMaxWidth", 16)}

    std::shared_ptr<cTransform> mTransform;
    std::shared_ptr<cAseprite> mAseprite;

private:
    int mMaxWidth{};
};

#endif // HEXABLO_OCHARACTERHEALTHBAR_H
