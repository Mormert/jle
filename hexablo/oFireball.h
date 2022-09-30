// Copyright (c) 2022. Johan Lind

#ifndef HEXABLO_OFIREBALL_H
#define HEXABLO_OFIREBALL_H

#include "jleObject.h"
#include "cTransform.h"
#include "cAseprite.h"


class oFireball : public jleObject {
    JLE_REGISTER_OBJECT_TYPE(oFireball)
public:

    oFireball();

    void SetupDefaultObject() override;

    void Start() override;

    void Update(float dt) override;

    void ToJson(nlohmann::json &j_out) override;

    void FromJson(const nlohmann::json &j_in) override;

    void ToNet(nlohmann::json &j_out);

    void FromNet(const nlohmann::json &j_in);


    void SetTarget(int q, int r);

private:
    std::shared_ptr<cTransform> mTransform{nullptr};
    std::shared_ptr<cAseprite> mAseprite{nullptr};

    int mMovingTowardsR, mMovingTowardsQ, mHexagonPixelX, mHexagonPixelY, mInterpingX, mInterpingY;
    const float mInterpBetweenHexasSpeed = 1.f;
    float mInterpingAlpha = 0.f;
    bool mInterpingPosition = true;

    int mId;

};

#endif //HEXABLO_OFIREBALL_H
