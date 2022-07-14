// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleComponent.h"
#include "jleObject.h"

class cTransform : public jle::jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cTransform)
public:

    explicit cTransform(jle::jleObject *owner = nullptr, jle::jleScene *scene = nullptr);

    inline void SetWorldPosition(float x, float y, float depth = 0.f) {

        mX = 0.f;
        mY = 0.f;
        mDepth = 0.f;

        auto p = mAttachedToObject->GetParent();
        while (p) {
            if (auto &&t = p->GetComponent<cTransform>()) {
                mX -= t->mX;
                mY -= t->mY;
                mDepth -= t->mDepth;
            }

            p = p->GetParent();
        }

        mX += x;
        mY += y;
        mDepth += depth;

        SetDirty();

    }

    inline void SetWorldPositionX(float x) {
        mX = 0.f;

        auto p = mAttachedToObject->GetParent();
        while (p) {
            if (auto &&t = p->GetComponent<cTransform>()) {
                mX -= t->mX;
            }

            p = p->GetParent();
        }

        mX += x;

        SetDirty();
    }

    inline void SetWorldPositionY(float y) {

        mY = 0.f;

        auto p = mAttachedToObject->GetParent();
        while (p) {
            if (auto &&t = p->GetComponent<cTransform>()) {
                mY -= t->mY;
            }

            p = p->GetParent();
        }

        mY += y;

        SetDirty();
    }

    inline void SetLocalPosition(float x, float y, float depth = 0.f) {
        mX = x, mY = y;
        mDepth = depth;

        SetDirty();
    }

    inline void AddLocalPosition(float x, float y, float depth = 0.f) {
        mX += x;
        mY += y;
        mDepth += depth;

        SetDirty();
    }

    inline void SetLocalPositionX(float x) {
        mX = x;
        SetDirty();
    }

    inline void SetLocalPositionY(float y) {
        mY = y;
        SetDirty();
    }

    [[nodiscard]] inline float GetWorldX() {
        if (mDirty) {
            RefreshWorldCoordinates();
        }

        return mWorldX;
    }

    [[nodiscard]] inline float GetWorldY() {
        if (mDirty) {
            RefreshWorldCoordinates();
        }

        return mWorldY;
    }

    [[nodiscard]] inline float GetWorldDepth() {
        if (mDirty) {
            RefreshWorldCoordinates();
        }

        return mWorldDepth;
    }

    [[nodiscard]] inline std::tuple<float,float,float> GetWorldXYDepth()
    {
        if (mDirty) {
            RefreshWorldCoordinates();
        }

        return std::make_tuple(mX, mY, mDepth);
    }

    [[nodiscard]] inline float GetLocalX() const {
        return mX;
    }

    [[nodiscard]] inline float GetLocalY() const {
        return mY;
    }

    [[nodiscard]] inline float GetLocalDepth() const { return mDepth; }

    inline void SetDirty() {
        mDirty = true;

        // Also set all the child transforms dirty
        auto &children = mAttachedToObject->GetChildObjects();
        for (auto &&child: children) {
            if (auto t = child->GetComponent<cTransform>()) {
                t->mDirty = true;
                t->SetDirty();
            }
        }
    }

    void ToJson(nlohmann::json &j_out) override;

    void FromJson(const nlohmann::json &j_in) override;

private:

    inline void RefreshWorldCoordinates() {
        mWorldX = mX;
        mWorldY = mY;
        mWorldDepth = mDepth;
        auto p = mAttachedToObject->GetParent();
        while (p) {
            if (auto &&t = p->GetComponent<cTransform>()) {
                mWorldX += t->mX;
                mWorldY += t->mY;
                mWorldDepth += t->mDepth;
            }

            p = p->GetParent();
        }
        mDirty = false;
    }

    float mX = 0.f, mY = 0.f, mDepth = 0.f;

    float mWorldX{}, mWorldY{}, mWorldDepth{};
    bool mDirty = true;
};