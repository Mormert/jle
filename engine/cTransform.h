// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleComponent.h"
#include "jleObject.h"

#include <glm/glm.hpp>

class cTransform : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cTransform)
public:
    explicit cTransform(jleObject *owner = nullptr, jleScene *scene = nullptr);

    inline void
    setWorldPosition(const glm::vec3 &position)
    {
        glm::vec3 newPos{0.f};

        auto p = _attachedToObject->parent();
        while (p) {
            if (auto &&t = p->component<cTransform>()) {
                const auto parentLocalPos = t->getLocalPosition();
                newPos -= parentLocalPos;
            }
            p = p->parent();
        }

        newPos += position;
        _transformMatLocal[3][0] = newPos.x;
        _transformMatLocal[3][1] = newPos.y;
        _transformMatLocal[3][2] = newPos.z;

        flagDirty();
    }

    inline void
    setLocalPosition(const glm::vec3 &position)
    {
        _transformMatLocal[3][0] = position.x;
        _transformMatLocal[3][1] = position.y;
        _transformMatLocal[3][2] = position.z;

        flagDirty();
    }

    inline void addLocalPosition(const glm::vec3 position) {
        _transformMatLocal[3][0] += position.x;
        _transformMatLocal[3][1] += position.y;
        _transformMatLocal[3][2] += position.z;

        flagDirty();
    }

    [[nodiscard]] inline glm::vec3
    getLocalPosition()
    {
        return glm::vec3{_transformMatLocal[3]};
    }

    [[nodiscard]] inline glm::vec3
    getWorldPosition()
    {
        if (_dirty) {
            refreshWorldCoordinates();
        }
        return glm::vec3{_transformMatWorld[3]};
    }

    [[nodiscard]] inline const glm::mat4 &
    getWorldMatrix()
    {
        if (_dirty) {
            refreshWorldCoordinates();
        }
        return _transformMatWorld;
    }

    [[nodiscard]] inline const glm::mat4 &
    getLocalMatrix()
    {
        if (_dirty) {
            refreshWorldCoordinates();
        }
        return _transformMatLocal;
    }

    inline void
    flagDirty()
    {
        _dirty = true;

        // Also set all the child transforms dirty
        auto &children = _attachedToObject->childObjects();
        for (auto &&child : children) {
            if (auto t = child->component<cTransform>()) {
                t->_dirty = true;
                t->flagDirty();
            }
        }
    }

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

private:
    inline void refreshWorldCoordinates() {
        _transformMatWorld = _transformMatLocal;
        auto p = _attachedToObject->parent();
        while (p) {
            if (auto &&t = p->component<cTransform>()) {
                const auto parentMat = t->_transformMatLocal;
                _transformMatWorld *= parentMat;
            }

            p = p->parent();
        }
        _dirty = false;
    }

    glm::mat4 _transformMatLocal{1.f};
    glm::mat4 _transformMatWorld{1.f};

    bool _dirty = true;
};