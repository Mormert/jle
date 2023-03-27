// Copyright (c) 2023. Johan Lind

#include "jleTransform.h"
#include "jleObject.h"

jleTransform::jleTransform(jleObject *owner){
    _owner = owner;
}

void
jleTransform::setWorldPosition(const glm::vec3 &position)
{
    glm::vec3 newPos{0.f};

    if (auto p = _owner->parent()) {
        const auto &parentWorldPos = p->getTransform().getWorldPosition();
        newPos -= parentWorldPos;
    }

    newPos += position;
    _local[3][0] = newPos.x;
    _local[3][1] = newPos.y;
    _local[3][2] = newPos.z;

    propagateMatrix();
}
void
jleTransform::setWorldMatrix(const glm::mat4 &matrix)
{

    if (auto p = _owner->parent()) {
        const auto &parentWorld = p->getTransform().getWorldMatrix();
        _local = glm::inverse(parentWorld) * matrix;
        propagateMatrix();
        return;
    }

    _local = matrix;
    propagateMatrix();
}
void
jleTransform::setLocalPosition(const glm::vec3 &position)
{
    _local[3][0] = position.x;
    _local[3][1] = position.y;
    _local[3][2] = position.z;

    propagateMatrix();
}
void
jleTransform::addLocalTranslation(const glm::vec3& position)
{
    _local[3][0] += position.x;
    _local[3][1] += position.y;
    _local[3][2] += position.z;

    propagateMatrix();
}
glm::vec3
jleTransform::getLocalPosition()
{
    return glm::vec3{_local[3]};
}
glm::vec3
jleTransform::getWorldPosition()
{
    return glm::vec3{_world[3]};
}
const glm::mat4 &
jleTransform::getWorldMatrix()
{
    return _world;
}

[[nodiscard]] const glm::mat4 &
jleTransform::getLocalMatrix()
{
    return _local;
}

const glm::mat4 &
jleTransform::getParentWorld()
{
    if (auto p = _owner->parent()) {
        return p->getTransform()._world;
    }
    static const glm::mat4 identityReturn{1.f};
    return identityReturn;
}

void
jleTransform::propagateMatrix()
{
    _world = getParentWorld() * getLocalMatrix();

    auto &children = _owner->childObjects();
    for (auto &&child : children) {
        child->getTransform().propagateMatrix();
    }
}
void
jleTransform::setLocalMatrix(const glm::mat4 &matrix)
{
    _local = matrix;
    propagateMatrix();
}
