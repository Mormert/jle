/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "jleTransform.h"
#include "core/jleObject.h"

#include "core/jleSceneClient.h"
#include "core/jleSceneServer.h"
//#include "modules/networking/jleNetworkEvent.h"

#include <glm/detail/type_quat.hpp>
#include <glm/gtx/matrix_decompose.hpp>

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleTransform)

/*struct jleTransformPropagateEvent : public jleServerToClientEvent {
    void
    execute(jleEngineModulesContext& ctx) override
    {
        auto &scene = getSceneClient();
        if (auto object = scene.getObjectFromNetId(netId)) {
            object->getTransform().setLocalMatrix(localMatrix);
        }
    }

    template <class Archive>
    void
    serialize(Archive &archive)
    {
        archive(CEREAL_NVP(localMatrix), CEREAL_NVP(netId));
    }

    glm::mat4 localMatrix;
    int32_t netId;
};

JLE_REGISTER_NET_EVENT(jleTransformPropagateEvent)
*/

jleTransform::
jleTransform(jleObject *owner)
{
    _owner = owner;
}

template <class Archive>
void
jleTransform::serialize(Archive &arcc)
{
    arcc(CEREAL_NVP(_local));
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
jleTransform::addLocalTranslation(const glm::vec3 &position)
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
    static constexpr glm::mat4 identityReturn{1.f};
    return identityReturn;
}

void
jleTransform::propagateMatrixFromObjectSerialization()
{
    _world = getParentWorld() * getLocalMatrix();

    auto &children = _owner->childObjects();
    for (auto &&child : children) {
        child->getTransform().propagateMatrix();
    }
}

void
jleTransform::propagateMatrix()
{
    _world = getParentWorld() * getLocalMatrix();

    auto &children = _owner->childObjects();
    for (auto &&child : children) {
        child->getTransform().propagateMatrixChildren();
    }

    /*
    if (_owner && _owner->networkObjectType() == jleObjectNetworkType::SERVER) {
        auto event = jleMakeNetEvent<jleTransformPropagateEvent>();
        event->localMatrix = getLocalMatrix();
        event->netId = _owner->netID();
        _owner->_containedInSceneServer->sendNetworkEventBroadcast(std::move(event));
    }
    */
}

void
jleTransform::propagateMatrixChildren()
{
    _world = getParentWorld() * getLocalMatrix();

    auto &children = _owner->childObjects();
    for (auto &&child : children) {
        child->getTransform().propagateMatrixChildren();
    }
}

void
jleTransform::setLocalMatrix(const glm::mat4 &matrix)
{
    jleAssertOnce(!glm::isnan(matrix[0][0]) && "Matrix should not contain NaN values");

    _local = matrix;
    propagateMatrix();
}

glm::vec3
jleTransform::getForward()
{
    return glm::normalize(glm::vec3{getWorldMatrix()[2]});
}

glm::vec3
jleTransform::getUp()
{
    return glm::normalize(glm::vec3{getWorldMatrix()[1]});
}

glm::vec3
jleTransform::getRight()
{
    return glm::normalize(glm::vec3{getWorldMatrix()[0]});
}

void
jleTransform::removeRotations()
{
    glm::vec3 translation, skew, scale;
    glm::quat rotation;
    glm::vec4 perspective;
    glm::decompose(getLocalMatrix(), scale, rotation, translation, skew, perspective);

    glm::mat4 newModelMatrix = glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), scale);
    setLocalMatrix(newModelMatrix);
}

void
jleTransform::rotateTowardsPoint(const glm::vec3 &position, const glm::vec3 &up)
{
    const auto &pos = getLocalPosition();
    glm::mat4 viewMatrix = glm::lookAt(pos, position, up);
    glm::mat4 rotationMatrix = glm::inverse(viewMatrix);
    setLocalMatrix(rotationMatrix);
}
