// Copyright (c) 2023. Johan Lind

#include "cRigidbody.h"
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <glm/ext/matrix_transform.hpp>

#include "jlePhysics.h"

#include "cMesh.h"
#include "jleGameEngine.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cRigidbody)


cRigidbody::cRigidbody(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void
cRigidbody::editorUpdate(float dt)
{
}

void
cRigidbody::start()
{
    if (_mass == 0) {
        generateCollisionStaticConcave();
    } else {
        generateCollisionDynamicConvex();
    }
}

void
cRigidbody::update(float dt)
{
}

btRigidBody *
cRigidbody::getBody()
{
    return body;
}

void
cRigidbody::generateCollisionDynamicConvex()
{
    btTransform transform;
    transform.setFromOpenGLMatrix((btScalar *)&getTransform().getWorldMatrix());

    const auto &&mesh = _attachedToObject->getComponent<cMesh>()->getMesh();

    _size.x = glm::length(glm::vec3(getTransform().getWorldMatrix()[0])); // Basis vector X
    _size.y = glm::length(glm::vec3(getTransform().getWorldMatrix()[1])); // Basis vector Y
    _size.z = glm::length(glm::vec3(getTransform().getWorldMatrix()[2])); // Basis vector Z

    auto shape = new btConvexHullShape(
        (const btScalar *)(&(mesh->positions()[0].x)), mesh->positions().size(), sizeof(glm::vec3));

    btVector3 localScaling(_size.x, _size.y, _size.z);
    shape->setLocalScaling(localScaling);

    // shape->optimizeConvexHull();

    // shape->initializePolyhedralFeatures();

    body = gEngine->physics().createRigidbody(_mass, transform, shape, this);
}

void
cRigidbody::generateCollisionStaticConcave()
{

    const auto &&mesh = _attachedToObject->getComponent<cMesh>()->getMesh();
    const auto &positions = mesh->positions();
    const auto &indices = mesh->indices();

    _size.x = glm::length(glm::vec3(getTransform().getWorldMatrix()[0])); // Basis vector X
    _size.y = glm::length(glm::vec3(getTransform().getWorldMatrix()[1])); // Basis vector Y
    _size.z = glm::length(glm::vec3(getTransform().getWorldMatrix()[2])); // Basis vector Z

    btTransform transform;
    auto mat = getTransform().getWorldMatrix();
    mat = glm::scale(mat, 1.f / _size); // Remove scaling
    transform.setFromOpenGLMatrix((btScalar *)&mat);

    auto *meshInterface = new btTriangleMesh();

    if (mesh->usesIndexing()) {
        for (int i = 0; i < indices.size() / 3; i++) {
            btVector3 v0 =
                btVector3{positions[indices[i * 3]].x, positions[indices[i * 3]].y, positions[indices[i * 3]].z};
            btVector3 v1 = btVector3{
                positions[indices[i * 3 + 1]].x, positions[indices[i * 3 + 1]].y, positions[indices[i * 3 + 1]].z};
            btVector3 v2 = btVector3{
                positions[indices[i * 3 + 2]].x, positions[indices[i * 3 + 2]].y, positions[indices[i * 3 + 2]].z};

            // Make sure to check that the triangle is large enough to have a normal calculated from it,
            // else we won't add it. For very small triangles, precision errors will cause the normal to have length 0.
            btVector3 normal = (v1 - v0).cross(v2 - v0);
            if (!normal.fuzzyZero()) {
                meshInterface->addTriangle(v0, v1, v2);
            }
        }
    } else {
        for (int i = 0; i < positions.size() / 3; i++) {
            btVector3 v0 = btVector3{positions[i * 3].x, positions[i * 3].y, positions[i * 3].z};
            btVector3 v1 = btVector3{positions[i * 3 + 1].x, positions[i * 3 + 1].y, positions[i * 3 + 1].z};
            btVector3 v2 = btVector3{positions[i * 3 + 2].x, positions[i * 3 + 2].y, positions[i * 3 + 2].z};

            btVector3 normal = (v1 - v0).cross(v2 - v0);
            if (!normal.fuzzyZero()) {
                meshInterface->addTriangle(v0, v1, v2);
            }
        }
    }

    auto shape = new btBvhTriangleMeshShape(meshInterface, true, true);

    shape->setLocalScaling(btVector3{_size.x, _size.y, _size.z});

    body = gEngine->physics().createRigidbody(_mass, transform, shape, this);
}
void
cRigidbody::onDestroy()
{
    gEngine->physics().deleteRigidbody(body);
}
