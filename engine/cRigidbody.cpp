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

#include "cMesh.h"
#include "jleGameEngine.h"
#include "jlePhysics.h"

#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h>

#include <glm/ext/matrix_transform.hpp>

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cRigidbody)

cRigidbody::cRigidbody(const cRigidbody &other) : jleComponent(other)
{
    _mass = other._mass;

    // Explicitly run setup when component cloning
    if (other._body) {
        setupRigidbody();
    }
}

void
cRigidbody::editorUpdate(float dt)
{
}

void
cRigidbody::start()
{
    setupRigidbody();
}

void
cRigidbody::update(float dt)
{
}

void
cRigidbody::getWorldTransform(btTransform &centerOfMassWorldTrans) const
{
    centerOfMassWorldTrans.setFromOpenGLMatrix(
        (btScalar *)&const_cast<cRigidbody *>(this)->object()->getTransform().getWorldMatrix()[0]);

    const_cast<cRigidbody *>(this)->_size.x =
        glm::length(glm::vec3(const_cast<cRigidbody *>(this)->getTransform().getWorldMatrix()[0])); // Basis vector X
    const_cast<cRigidbody *>(this)->_size.y =
        glm::length(glm::vec3(const_cast<cRigidbody *>(this)->getTransform().getWorldMatrix()[1])); // Basis vector Y
    const_cast<cRigidbody *>(this)->_size.z =
        glm::length(glm::vec3(const_cast<cRigidbody *>(this)->getTransform().getWorldMatrix()[2])); // Basis vector Z
}

void
cRigidbody::setWorldTransform(const btTransform &centerOfMassWorldTrans)
{
    glm::mat4 matrix;
    centerOfMassWorldTrans.getOpenGLMatrix((btScalar *)&matrix);
    matrix = glm::scale(matrix, _size);
    object()->getTransform().setWorldMatrix(matrix);
}

std::unique_ptr<btRigidBody>
cRigidbody::createRigidbody(bool isDynamic, btCollisionShape *shape)
{
    jleAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

    btVector3 localInertia(0, 0, 0);
    if (isDynamic) {
        shape->calculateLocalInertia(_mass, localInertia);
    } else {
        _size.x = glm::length(glm::vec3(getTransform().getWorldMatrix()[0])); // Basis vector X
        _size.y = glm::length(glm::vec3(getTransform().getWorldMatrix()[1])); // Basis vector Y
        _size.z = glm::length(glm::vec3(getTransform().getWorldMatrix()[2])); // Basis vector Z

        auto v = btVector3{_size.x, _size.y, _size.z};
        _optionalLocalShape =
            std::make_unique<btScaledBvhTriangleMeshShape>(reinterpret_cast<btBvhTriangleMeshShape *>(shape), v);
        shape = _optionalLocalShape.get();
    }

    btRigidBody::btRigidBodyConstructionInfo cInfo(_mass, this, shape, localInertia);

    auto body = std::make_unique<btRigidBody>(cInfo);

    body->setUserIndex(-1);
    body->setUserPointer(this);

    return body;
}

void
cRigidbody::setupRigidbody()
{
    // Rigidbody is dynamic if and only if mass is non-zero, otherwise static
    bool isDynamic = (_mass != 0.f);

    if (isDynamic) {
        const auto &dynamicConvexShape = object()->getComponent<cMesh>()->getMesh()->getDynamicConvexShape();
        _body = createRigidbody(isDynamic, dynamicConvexShape);
    } else {
        const auto &staticConcaveShape = object()->getComponent<cMesh>()->getMesh()->getStaticConcaveShape();
        _body = createRigidbody(isDynamic, staticConcaveShape);
    }

    scene()->getPhysics().addRigidbody(_body.get());

    setWorldMatrixAndScaleRigidbody(getTransform().getWorldMatrix());

    _body->activate();
}

void
cRigidbody::setupNewRigidbodyAndDeleteOld()
{
    scene()->getPhysics().removeRigidbody(_body.get());
    setupRigidbody();
}

bool
cRigidbody::isDynamic()
{
    return _mass != 0.f;
}

void
cRigidbody::onDestroy()
{
    scene()->getPhysics().removeRigidbody(_body.get());
}

btRigidBody &
cRigidbody::getBody()
{
    return *_body.get();
}

void
cRigidbody::setWorldMatrixAndScaleRigidbody(const glm::mat4 &worldMatrix)
{
    if (isDynamic()) {
        getTransform().setWorldMatrix(worldMatrix);

        setupNewRigidbodyAndDeleteOld();
        glm::vec3 size;

        size.x = glm::length(glm::vec3(worldMatrix[0])); // Basis vector X
        size.y = glm::length(glm::vec3(worldMatrix[1])); // Basis vector Y
        size.z = glm::length(glm::vec3(worldMatrix[2])); // Basis vector Z
        getBody().getCollisionShape()->setLocalScaling({size.x, size.y, size.z});

    } else {
        // Remove scaling from the world matrix (bullet don't want the scaling for static objects)
        glm::vec3 size;
        size.x = glm::length(glm::vec3(worldMatrix[0])); // Basis vector X
        size.y = glm::length(glm::vec3(worldMatrix[1])); // Basis vector Y
        size.z = glm::length(glm::vec3(worldMatrix[2])); // Basis vector Z

        getTransform().setWorldMatrix(worldMatrix);

        const glm::mat4 scaledMatrix = glm::scale(worldMatrix, glm::vec3(1.f / size.x, 1.f / size.y, 1.f / size.z));

        btTransform transform;
        transform.setFromOpenGLMatrix((btScalar *)&scaledMatrix);

        getBody().setWorldTransform(transform);
        getBody().getCollisionShape()->setLocalScaling({size.x, size.y, size.z});
    }
}
