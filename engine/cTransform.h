// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleComponent.h"
#include "jleObject.h"

#include <glm/glm.hpp>

class cTransform : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cTransform)
public:
    explicit cTransform(jleObject *owner = nullptr, jleScene *scene = nullptr);

    inline void SetWorldPosition(float x, float y, float depth) {

        _x = 0.f;
        _y = 0.f;
        _depth = 0.f;

        auto p = _attachedToObject->GetParent();
        while (p) {
            if (auto&& t = p->GetComponent<cTransform>()) {
                _x -= t->_x;
                _y -= t->_y;
                _depth -= t->_depth;
            }

            p = p->GetParent();
        }

        _x += x;
        _y += y;
        _depth += depth;

        SetDirty();
    }

    inline void SetWorldPositionXY(float x, float y) {

        _x = 0.f;
        _y = 0.f;

        auto p = _attachedToObject->GetParent();
        while (p) {
            if (auto&& t = p->GetComponent<cTransform>()) {
                _x -= t->_x;
                _y -= t->_y;
            }

            p = p->GetParent();
        }

        _x += x;
        _y += y;

        SetDirty();
    }

    inline void SetWorldPositionX(float x) {
        _x = 0.f;

        auto p = _attachedToObject->GetParent();
        while (p) {
            if (auto&& t = p->GetComponent<cTransform>()) {
                _x -= t->_x;
            }

            p = p->GetParent();
        }

        _x += x;

        SetDirty();
    }

    inline void SetWorldPositionY(float y) {

        _y = 0.f;

        auto p = _attachedToObject->GetParent();
        while (p) {
            if (auto&& t = p->GetComponent<cTransform>()) {
                _y -= t->_y;
            }

            p = p->GetParent();
        }

        _y += y;

        SetDirty();
    }

    inline void SetLocalPosition(float x, float y, float depth = 0.f) {
        _x = x, _y = y;
        _depth = depth;

        SetDirty();
    }

    inline void AddLocalPosition(float x, float y, float depth = 0.f) {
        _x += x;
        _y += y;
        _depth += depth;

        SetDirty();
    }

    inline void SetLocalPositionX(float x) {
        _x = x;
        SetDirty();
    }

    inline void SetLocalPositionY(float y) {
        _y = y;
        SetDirty();
    }

    [[nodiscard]] inline float GetWorldX() {
        if (_dirty) {
            RefreshWorldCoordinates();
        }

        return _worldX;
    }

    [[nodiscard]] inline float GetWorldY() {
        if (_dirty) {
            RefreshWorldCoordinates();
        }

        return _worldY;
    }

    [[nodiscard]] inline float GetWorldDepth() {
        if (_dirty) {
            RefreshWorldCoordinates();
        }

        return _worldDepth;
    }

    [[nodiscard]] inline glm::vec3 GetWorldXYDepth() {
        if (_dirty) {
            RefreshWorldCoordinates();
        }

        return {_x, _y, _depth};
    }

    [[nodiscard]] inline float GetLocalX() const { return _x; }

    [[nodiscard]] inline float GetLocalY() const { return _y; }

    [[nodiscard]] inline float GetLocalDepth() const { return _depth; }

    inline void SetDirty() {
        _dirty = true;

        // Also set all the child transforms dirty
        auto& children = _attachedToObject->GetChildObjects();
        for (auto&& child : children) {
            if (auto t = child->GetComponent<cTransform>()) {
                t->_dirty = true;
                t->SetDirty();
            }
        }
    }

    void ToJson(nlohmann::json& j_out) override;

    void FromJson(const nlohmann::json& j_in) override;

private:
    inline void RefreshWorldCoordinates() {
        _worldX = _x;
        _worldY = _y;
        _worldDepth = _depth;
        auto p = _attachedToObject->GetParent();
        while (p) {
            if (auto&& t = p->GetComponent<cTransform>()) {
                _worldX += t->_x;
                _worldY += t->_y;
                _worldDepth += t->_depth;
            }

            p = p->GetParent();
        }
        _dirty = false;
    }

    float _x = 0.f, _y = 0.f, _depth = 0.f;

    float _worldX{}, _worldY{}, _worldDepth{};
    bool _dirty = true;
};