// Copyright (c) 2022. Johan Lind

#pragma once

#include "cAseprite.h"
#include "cTransform.h"
#include "jleObject.h"

class oFireball : public jleObject {
    JLE_REGISTER_OBJECT_TYPE(oFireball)
public:
    oFireball();

    void setupDefaultObject() override;

    void start() override;

    void update(float dt) override;

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

    void toNet(nlohmann::json &j_out);

    void fromNet(const nlohmann::json &j_in);

    void target(int q, int r);

private:
    std::shared_ptr<cTransform> _transform{nullptr};
    std::shared_ptr<cAseprite> _aseprite{nullptr};

    int _movingTowardsR, _movingTowardsQ, _hexagonPixelX, _hexagonPixelY,
        _interpingX, _interpingY;
    const float _interpBetweenHexasSpeed = 1.f;
    float _interpingAlpha = 0.f;
    bool _interpingPosition = true;

    int _id;
};
