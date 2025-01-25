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

#include "cRigidbody.h"

#include "modules/graphics/runtime/components/cMesh.h"
#include "modules/physics/jlePhysics.h"
#include "modules/hierarchy/components/cTransform.h"

#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include "BulletDynamics/Dynamics/btRigidBody.h"


#include <glm/ext/matrix_transform.hpp>
#include <jlECS/jlECS.h>
#include <modules/hierarchy/jleHierarchyFuncs.h>


void cRigidbodyMotionState::getWorldTransform(btTransform &worldMatrixBullet) const {
    auto objectRef = _ecs->getObject(_objectIndex);
    if (auto transformPtr = objectRef.getComponentPtr<cTransform>()) {
        const glm::mat4 worldMatrixJle = jleHierarchyFuncs::getWorldMatrix(objectRef);
        worldMatrixBullet.setFromOpenGLMatrix((btScalar *)&worldMatrixJle[0]);
    }
}

void cRigidbodyMotionState::setWorldTransform(const btTransform &worldMatrixBullet) {
    auto objectRef = _ecs->getObject(_objectIndex);
    if (auto transformPtr = objectRef.getComponentPtr<cTransform>()) {
        glm::mat4 worldMatrixJle;
        worldMatrixBullet.getOpenGLMatrix((btScalar *)&worldMatrixJle);
        jleHierarchyFuncs::setLocalMatrixFromWorld(objectRef, worldMatrixJle);
    }
}

cRigidbody::cRigidbody() = default;
cRigidbody::~cRigidbody() = default;

cRigidbody::cRigidbody(cRigidbody&& other) noexcept
    : _mass(other._mass),
      _body(other._body),
      _optionalLocalShape(std::move(other._optionalLocalShape)),
      _motionState(std::move(other._motionState)){
}

cRigidbody& cRigidbody::operator=(cRigidbody&& other) noexcept {
    if (this != &other) {
        _mass = other._mass;
        _body = other._body;
        _optionalLocalShape = std::move(other._optionalLocalShape);
    }
    return *this;
}

bool
cRigidbody::isDynamic()
{
    return _mass != 0.f;
}

btRigidBody &
cRigidbody::getBody()
{
    assert(_body);
    return *_body;
}
