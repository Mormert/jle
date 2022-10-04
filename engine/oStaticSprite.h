// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleObject.h"

#include "cSprite.h"
#include "cTransform.h"

#include <iostream>
#include <memory>

class oStaticSprite : public jleObject {
    JLE_REGISTER_OBJECT_TYPE(oStaticSprite)
public:
    void upDefaultObject() override;

    void start() override;

    void update(float dt) override;

    void toJson(nlohmann::json& j_out) override {

        transform->toJson(j_out["transform"]);
        sprite->toJson(j_out["sprite"]);
    }

    void fromJson(const nlohmann::json& j_in) override {

        try {
            transform->fromJson(j_in.at("transform"));
            sprite->fromJson(j_in.at("sprite"));
        }
        catch (std::exception e) {
            std::cout << e.what();
        }
    }

    std::shared_ptr<cTransform> transform;
    std::shared_ptr<cSprite> sprite;

private:
    static inline int beginX = 0, beginY = 0;
};