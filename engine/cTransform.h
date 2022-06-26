// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleComponent.h"

class cTransform : public jle::jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cTransform)
public:

    explicit cTransform(jle::jleObject *owner = nullptr, jle::jleScene *scene = nullptr);

    inline void SetPosition(float x, float y, float depth = 0.f) {
        mX = x, mY = y;
        mDepth = depth;
    }

    inline void AddPosition(float x, float y, float depth = 0.f) {
        mX += x;
        mY += y;
        mDepth += depth;
    }

    inline void SetPositionX(float x) { mX = x; }

    inline void SetPositionY(float y) { mY = y; }

    [[nodiscard]] inline float GetX() const { return mX; }

    [[nodiscard]] inline float GetY() const { return mY; }

    [[nodiscard]] inline float GetDepth() const { return mDepth; }

    void ToJson(nlohmann::json &j_out) override;

    void FromJson(const nlohmann::json &j_in) override;

private:
    float mX = 0.f, mY = 0.f, mDepth = 0.f;
};