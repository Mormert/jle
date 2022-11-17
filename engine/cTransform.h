// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleComponent.h"
#include "jleObject.h"

#include <glm/glm.hpp>

class cTransform : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cTransform)
public:
    explicit cTransform(jleObject *owner = nullptr, jleScene *scene = nullptr);

    void start() override;

    inline void
    setWorldPosition(const glm::vec3 &position)
    {
        glm::vec3 newPos{0.f};

        auto p = _attachedToObject->parent();

        if (p) {
            if (auto &&t = p->component<cTransform>()) {
                const auto &parentWorldPos = t->getWorldPosition();
                newPos -= parentWorldPos;
            }
        }

        newPos += position;
        _local[3][0] = newPos.x;
        _local[3][1] = newPos.y;
        _local[3][2] = newPos.z;

        propagateMatrix();
    }

    inline void
    setWorldMatrix(const glm::mat4 &matrix)
    {

        auto p = _attachedToObject->parent();

        if (p) {
            if (auto &&t = p->component<cTransform>()) {
                const auto &parentWorld = t->getWorldMatrix();
                _local = glm::inverse(parentWorld) * matrix;
                propagateMatrix();
                return;
            }
        }

        _local = matrix;
        propagateMatrix();
    }

    inline void
    setLocalPosition(const glm::vec3 &position)
    {
        _local[3][0] = position.x;
        _local[3][1] = position.y;
        _local[3][2] = position.z;

        propagateMatrix();
    }

    inline void addLocalPosition(const glm::vec3 position)
    {
        _local[3][0] += position.x;
        _local[3][1] += position.y;
        _local[3][2] += position.z;

        propagateMatrix();
    }

    [[nodiscard]] inline glm::vec3
    getLocalPosition()
    {
        return glm::vec3{_local[3]};
    }

    [[nodiscard]] inline glm::vec3
    getWorldPosition()
    {
        return glm::vec3{_world[3]};
    }

    [[nodiscard]] inline const glm::mat4 &
    getWorldMatrix()
    {
        return _world;
    }

    [[nodiscard]] inline const glm::mat4 &
    getLocalMatrix()
    {
        return _local;
    }

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

private:
    inline glm::mat4 &
    getParentWorld()
    {
        if (auto p = _attachedToObject->parent()) {
            if (auto pt = p->component<cTransform>()) {
                return pt->_world;
            }
        }
        static glm::mat4 identityReturn{1.f};
        return identityReturn;
    }

    inline void
    propagateMatrix()
    {
        _world = getParentWorld() * getLocalMatrix();

        auto &children = _attachedToObject->childObjects();
        for (auto &&child : children) {
            if (auto t = child->component<cTransform>()) {
                t->propagateMatrix();
            }
        }
    }

    glm::mat4 _local{1.f};
    glm::mat4 _world{1.f};
};