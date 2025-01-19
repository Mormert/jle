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

#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>

#include <glm/ext/matrix_transform.hpp>

cRigidbody::cRigidbody() = default;
cRigidbody::~cRigidbody() = default;

cRigidbody::cRigidbody(cRigidbody&& other) noexcept
    : _mass(other._mass),
      _size(other._size),
      _body(std::move(other._body)),
      _optionalLocalShape(std::move(other._optionalLocalShape)) {
}

cRigidbody& cRigidbody::operator=(cRigidbody&& other) noexcept {
    if (this != &other) {
        _mass = other._mass;
        _size = other._size;
        _body = std::move(other._body);
        _optionalLocalShape = std::move(other._optionalLocalShape);
    }
    return *this;
}

void cRigidbody::duplicate(jlePhysics* physics, cRigidbody *source, cRigidbody *dest) {
    dest->_mass = source->_mass;
    if (source->_body) {
        assert(false);
        // Todo : fix this
        //dest->setupRigidbody();
    }
}

std::unique_ptr<btRigidBody>
cRigidbody::createRigidbody(bool isDynamic, const cTransform& transform, btCollisionShape *shape)
{
    jleAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

    btVector3 localInertia(0, 0, 0);
    if (isDynamic) {
        shape->calculateLocalInertia(_mass, localInertia);
    } else {
        _size.x = glm::length(glm::vec3(transform.getLocalMatrix()[0])); // Basis vector X
        _size.y = glm::length(glm::vec3(transform.getLocalMatrix()[1])); // Basis vector Y
        _size.z = glm::length(glm::vec3(transform.getLocalMatrix()[2])); // Basis vector Z

        auto v = btVector3{_size.x, _size.y, _size.z};
        _optionalLocalShape = std::make_unique<btScaledBvhTriangleMeshShape>(reinterpret_cast<btBvhTriangleMeshShape *>(shape), v);
        shape = _optionalLocalShape.get();
    }

    btRigidBody::btRigidBodyConstructionInfo cInfo{
        _mass, nullptr, shape, localInertia
    };

    auto body = std::make_unique<btRigidBody>(cInfo);

    body->setUserIndex(-1);
    body->setUserPointer(this);

    return body;
}

void
cRigidbody::setupRigidbody(jlePhysics* physics, cTransform& transform, cMesh& mesh)
{
    // Rigidbody is dynamic if and only if mass is non-zero, otherwise static
    bool isDynamic = (_mass != 0.f);

    if (isDynamic) {
        btConvexHullShape *dynamicConvexShape = mesh.getMesh()->getDynamicConvexShape();
        _body = createRigidbody(isDynamic, transform, dynamicConvexShape);
    } else {
        const auto &staticConcaveShape = mesh.getMesh()->getStaticConcaveShape();
        _body = createRigidbody(isDynamic, transform, staticConcaveShape);
    }

    physics->addRigidbody(_body.get());

    setWorldMatrixAndScaleRigidbody(physics, transform, mesh);

    _body->activate();
}


bool
cRigidbody::isDynamic()
{
    return _mass != 0.f;
}

btRigidBody &
cRigidbody::getBody()
{
    return *_body.get();
}

void
cRigidbody::setWorldMatrixAndScaleRigidbody(jlePhysics* physics, cTransform& transform, cMesh& mesh)
{
    const auto& worldMatrix = transform.getLocalMatrix();

    if (isDynamic()) {
        assert(false); // todo fix this
        // Remove old rigidbody
        physics->removeRigidbody(_body.get());
        // Setup new rigidbody
        setupRigidbody(physics, transform, mesh);

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

        transform.setLocalMatrix(worldMatrix);

        const glm::mat4 scaledMatrix = glm::scale(worldMatrix, glm::vec3(1.f / size.x, 1.f / size.y, 1.f / size.z));

        btTransform bulletTransform;
        bulletTransform.setFromOpenGLMatrix((btScalar *)&scaledMatrix);

        getBody().setWorldTransform(bulletTransform);
        getBody().getCollisionShape()->setLocalScaling({size.x, size.y, size.z});
    }
}
