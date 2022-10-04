// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleComponent.h"
#include "jleObject.h"

#include <glm/glm.hpp>

class cTransform : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cTransform)
public:
    explicit cTransform(jleObject *owner = nullptr, jleScene *scene = nullptr);

    inline void worldPosition(float x, float y, float depth) {

        _x = 0.f;
        _y = 0.f;
        _depth = 0.f;

        auto p = _attachedToObject->parent();
        while (p) {
            if (auto &&t = p->component<cTransform>()) {
                _x -= t->_x;
                _y -= t->_y;
                _depth -= t->_depth;
            }

            p = p->parent();
        }

        _x += x;
        _y += y;
        _depth += depth;

        dirty();
    }

    inline void worldPositionXY(float x, float y) {

        _x = 0.f;
        _y = 0.f;

        auto p = _attachedToObject->parent();
        while (p) {
            if (auto &&t = p->component<cTransform>()) {
                _x -= t->_x;
                _y -= t->_y;
            }

            p = p->parent();
        }

        _x += x;
        _y += y;

        dirty();
    }

    inline void worldPositionX(float x) {
        _x = 0.f;

        auto p = _attachedToObject->parent();
        while (p) {
            if (auto &&t = p->component<cTransform>()) {
                _x -= t->_x;
            }

            p = p->parent();
        }

        _x += x;

        dirty();
    }

    inline void worldPositionY(float y) {

        _y = 0.f;

        auto p = _attachedToObject->parent();
        while (p) {
            if (auto &&t = p->component<cTransform>()) {
                _y -= t->_y;
            }

            p = p->parent();
        }

        _y += y;

        dirty();
    }

    inline void localPosition(float x, float y, float depth = 0.f) {
        _x = x, _y = y;
        _depth = depth;

        dirty();
    }

    inline void addLocalPosition(float x, float y, float depth = 0.f) {
        _x += x;
        _y += y;
        _depth += depth;

        dirty();
    }

    inline void localPositionX(float x) {
        _x = x;
        dirty();
    }

    inline void localPositionY(float y) {
        _y = y;
        dirty();
    }

    [[nodiscard]] inline float worldX() {
        if (_dirty) {
            refreshWorldCoordinates();
        }

        return _worldX;
    }

    [[nodiscard]] inline float worldY() {
        if (_dirty) {
            refreshWorldCoordinates();
        }

        return _worldY;
    }

    [[nodiscard]] inline float worldDepth() {
        if (_dirty) {
            refreshWorldCoordinates();
        }

        return _worldDepth;
    }

    [[nodiscard]] inline glm::vec3 worldXYDepth() {
        if (_dirty) {
            refreshWorldCoordinates();
        }

        return {_x, _y, _depth};
    }

    [[nodiscard]] inline float localX() const { return _x; }

    [[nodiscard]] inline float localY() const { return _y; }

    [[nodiscard]] inline float localDepth() const { return _depth; }

    inline void dirty() {
        _dirty = true;

        // Also set all the child transforms dirty
        auto &children = _attachedToObject->childObjects();
        for (auto &&child : children) {
            if (auto t = child->component<cTransform>()) {
                t->_dirty = true;
                t->dirty();
            }
        }
    }

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

private:
    inline void refreshWorldCoordinates() {
        _worldX = _x;
        _worldY = _y;
        _worldDepth = _depth;
        auto p = _attachedToObject->parent();
        while (p) {
            if (auto &&t = p->component<cTransform>()) {
                _worldX += t->_x;
                _worldY += t->_y;
                _worldDepth += t->_depth;
            }

            p = p->parent();
        }
        _dirty = false;
    }

    float _x = 0.f, _y = 0.f, _depth = 0.f;

    float _worldX{}, _worldY{}, _worldDepth{};
    bool _dirty = true;
};