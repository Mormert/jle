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

#include "jlePhysicsModule.h"

#include <jlECS/jlECS.h>
#include <modules/graphics/runtime/components/cMesh.h>
#include <modules/hierarchy/jleHierarchyFuncs.h>
#include "modules/hierarchy/components/cTransform.h"

#include "components/cRigidbody.h"

#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btTriangleMesh.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"

jlePhysicsModule::jlePhysicsModule() {
    _physics = std::make_unique<jlePhysics>();
}

jlePhysicsModule::~jlePhysicsModule() = default;

void
jlePhysicsModule::initializeECS(jlECS::ECS &ecs)
{
    ecs.registerComponentType<cRigidbody>({
        .onCreateCallback = [this](jlECS::CreateComponentData& data) {
            onRigidbodyCreated(data);
        },
        .onDestroyCallback = [this](jlECS::DestroyComponentData& data) {
            onRigidbodyDestroyed(data);
        },
        .onDuplicateCallback = [this](void* sourceComp, void* destComp) {
            onRigidbodyCopied(static_cast<cRigidbody*>(sourceComp), static_cast<cRigidbody*>(destComp));
        }
    });
}

void
jlePhysicsModule::update(const jlePhysicsModule::UpdateContext &ctx)
{
    for (auto& createRigidbodyEvent : _rigidbodyCreateRequests) {
        auto* rb = static_cast<cRigidbody*>(createRigidbodyEvent.componentPtr);
        jlECS::ObjectRef& objectRef = createRigidbodyEvent.objectRef;

        if(!objectRef.getComponent<cTransform>().isValid())
        {
            objectRef.addComponent<cTransform>();
        }

        cMesh* meshOptional = objectRef.getComponentPtr<cMesh>();
        const glm::mat4& worldMatrix = ctx.in.worldMatrices[objectRef.objectIndex()];
        setupRigidbody(objectRef, rb, worldMatrix, meshOptional);
    }
    _rigidbodyCreateRequests.clear();

    for (auto& bulletRigidbody : _rigidbodyDestroyRequests) {
        _physics->removeRigidbody(bulletRigidbody);
        delete bulletRigidbody;
    }
    _rigidbodyDestroyRequests.clear();

    // Check for rigidbody scaling updates, or if moved from outside the physics system
   for(auto [objectIndex, rb] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cRigidbody>()){
       if (rb->_updateRigidbodyScaling) {
           const glm::mat4& worldMatrix = ctx.in.worldMatrices[objectIndex];
           auto objectRef = ctx.inOut.ecs.getObject(objectIndex);
           cMesh* meshOptional = objectRef.getComponentPtr<cMesh>();
           updateRigidbodyScaling(objectRef, rb, worldMatrix, meshOptional);
           rb->_updateRigidbodyScaling = false;
       }
   }

    // Update bullet physics
    _physics->step(ctx.in.dt);

}

void
jlePhysicsModule::onRigidbodyCreated(const jlECS::CreateComponentData &createCallbackData)
{
    _rigidbodyCreateRequests.push_back(createCallbackData);
}

void
jlePhysicsModule::onRigidbodyDestroyed(const jlECS::DestroyComponentData& destroyCallbackData)
{
    const cRigidbody* rigidbodyPtr = static_cast<cRigidbody*>(destroyCallbackData.componentPtr);
    _rigidbodyDestroyRequests.push_back(rigidbodyPtr->_body);
}

void jlePhysicsModule::onRigidbodyCopied(cRigidbody *source, cRigidbody *dest)
{
}

void jlePhysicsModule::setupRigidbody(jlECS::ObjectRef& objectRef, cRigidbody *rigidbodyPtr, const glm::mat4& worldMatrix, cMesh *meshOptional) {
    // Rigidbody is dynamic if and only if mass is non-zero, otherwise static
    bool isDynamic = (rigidbodyPtr->_mass != 0.f);

    switch (rigidbodyPtr->_shapeType) {
        case cRigidbody::cRigidbodyShapeType::USE_MESH:
            {
                if (meshOptional) {
                    if(isDynamic) {
                        const auto dynamicConvexShape = getDynamicConvexShape(meshOptional->getMesh());
                        rigidbodyPtr->_body = createDynamicRigidbody(objectRef, rigidbodyPtr, worldMatrix, dynamicConvexShape.get());
                    } else {
                        const auto staticConcaveShape = getStaticConcaveShape(meshOptional->getMesh());
                        rigidbodyPtr->_body = createStaticRigidbody(objectRef, rigidbodyPtr, worldMatrix, staticConcaveShape.get());
                    }
                }else
                {
                    // TODO: Add default cube or something as fallback
                    LOGE << "Rigidbody type is mesh, but no mesh component attached to object!";
                }
            }break;
        case cRigidbody::cRigidbodyShapeType::CUBE:
        case cRigidbody::cRigidbodyShapeType::SPHERE:
        default:
            LOGE << "Unsupported rigidbody shape type!";
            break;
    }

    _physics->addRigidbody(rigidbodyPtr->_body);
    rigidbodyPtr->_body->activate();
}

void jlePhysicsModule::updateRigidbodyScaling(jlECS::ObjectRef& objectRef, cRigidbody *rigidbodyPtr, const glm::mat4& worldMatrix, cMesh *meshOptional) {
    if (rigidbodyPtr->isDynamic()) {
        // Remove old rigidbody
        _physics->removeRigidbody(rigidbodyPtr->_body);
        delete rigidbodyPtr->_body;

        // Setup new rigidbody
        setupRigidbody(objectRef, rigidbodyPtr, worldMatrix, meshOptional);

        glm::vec3 size;

        size.x = glm::length(glm::vec3(worldMatrix[0])); // Basis vector X
        size.y = glm::length(glm::vec3(worldMatrix[1])); // Basis vector Y
        size.z = glm::length(glm::vec3(worldMatrix[2])); // Basis vector Z
        rigidbodyPtr->getBody().getCollisionShape()->setLocalScaling({size.x, size.y, size.z});

    } else {
        // Remove scaling from the world matrix (bullet don't want the scaling for static objects)
        glm::vec3 size;
        size.x = glm::length(glm::vec3(worldMatrix[0])); // Basis vector X
        size.y = glm::length(glm::vec3(worldMatrix[1])); // Basis vector Y
        size.z = glm::length(glm::vec3(worldMatrix[2])); // Basis vector Z

        jleHierarchyFuncs::setLocalMatrixFromWorld(objectRef, worldMatrix);

        const glm::mat4 scaledMatrix = glm::scale(worldMatrix, glm::vec3(1.f / size.x, 1.f / size.y, 1.f / size.z));

        btTransform bulletTransform;
        bulletTransform.setFromOpenGLMatrix((btScalar *)&scaledMatrix);

        rigidbodyPtr->getBody().setWorldTransform(bulletTransform);
        rigidbodyPtr->getBody().getCollisionShape()->setLocalScaling({size.x, size.y, size.z});
    }

}

std::shared_ptr<btConvexHullShape> jlePhysicsModule::getDynamicConvexShape(const std::shared_ptr<jleMesh> &meshPtr) {
    auto it = _dynamicConvexShapes.find(meshPtr);
    if (it == _dynamicConvexShapes.end()) {
        auto shape = std::make_shared<btConvexHullShape>((&(meshPtr->positions()[0].x)), (int)meshPtr->positions().size(), sizeof(glm::vec3));
        // shape->optimizeConvexHull();
        // shape->initializePolyhedralFeatures();
        _dynamicConvexShapes.emplace(meshPtr, shape);
        return shape;
    }
    return it->second;
}

std::shared_ptr<btBvhTriangleMeshShape> jlePhysicsModule::getStaticConcaveShape(const std::shared_ptr<jleMesh> &meshPtr) {

    auto it = _staticConcaveShapes.find(meshPtr);
    if (it == _staticConcaveShapes.end()) {
        auto staticConcaveShapeMeshInterface = std::make_shared<btTriangleMesh>();
        _staticConcaveShapeMeshInterfaces.emplace(meshPtr, staticConcaveShapeMeshInterface);

        const auto& positions = meshPtr->positions();
        const auto& indices = meshPtr->indices();

        if (meshPtr->usesIndexing()) {
            for (int i = 0; i < indices.size() / 3; i++) {
                btVector3 v0 =
                    btVector3{positions[indices[i * 3]].x, positions[indices[i * 3]].y, positions[indices[i * 3]].z};
                btVector3 v1 = btVector3{positions[indices[i * 3 + 1]].x,
                                         positions[indices[i * 3 + 1]].y,
                                         positions[indices[i * 3 + 1]].z};
                btVector3 v2 = btVector3{positions[indices[i * 3 + 2]].x,
                                         positions[indices[i * 3 + 2]].y,
                                         positions[indices[i * 3 + 2]].z};

                // Make sure to check that the triangle is large enough to have a normal calculated from it,
                // else we won't add it. For very small triangles, precision errors will cause the normal to have length 0.
                btVector3 normal = (v1 - v0).cross(v2 - v0);
                if (!normal.fuzzyZero()) {
                    staticConcaveShapeMeshInterface->addTriangle(v0, v1, v2);
                }
            }
        } else {
            for (int i = 0; i < positions.size() / 3; i++) {
                btVector3 v0 = btVector3{positions[i * 3].x, positions[i * 3].y, positions[i * 3].z};
                btVector3 v1 = btVector3{positions[i * 3 + 1].x, positions[i * 3 + 1].y, positions[i * 3 + 1].z};
                btVector3 v2 = btVector3{positions[i * 3 + 2].x, positions[i * 3 + 2].y, positions[i * 3 + 2].z};

                btVector3 normal = (v1 - v0).cross(v2 - v0);
                if (!normal.fuzzyZero()) {
                    staticConcaveShapeMeshInterface->addTriangle(v0, v1, v2);
                }
            }
        }

        auto shape = std::make_shared<btBvhTriangleMeshShape>(staticConcaveShapeMeshInterface.get(), true, true);
        _staticConcaveShapes.emplace(meshPtr, shape);
        return shape;
    }
    return it->second;
}

btRigidBody* jlePhysicsModule::createDynamicRigidbody(jlECS::ObjectRef& objectRef, cRigidbody *rigidbodyPtr, const glm::mat4 &worldMatrix, btCollisionShape *shape) {
    jleAssert(shape);
    jleAssert(shape->getShapeType() != INVALID_SHAPE_PROXYTYPE);

    btVector3 localInertia(0, 0, 0);
    shape->calculateLocalInertia(rigidbodyPtr->_mass, localInertia);

    rigidbodyPtr->_motionState = std::make_unique<cRigidbodyMotionState>(&objectRef.getECS(), objectRef.objectIndex());

    btRigidBody::btRigidBodyConstructionInfo cInfo{
        rigidbodyPtr->_mass, rigidbodyPtr->_motionState.get(), shape, localInertia
    };

    btRigidBody* body = new btRigidBody(cInfo);

    body->setUserIndex(-1);
    body->setUserPointer(rigidbodyPtr);

    return body;
}

btRigidBody* jlePhysicsModule::createStaticRigidbody(jlECS::ObjectRef& objectRef, cRigidbody *rigidbodyPtr, const glm::mat4& worldMatrix, btCollisionShape *shape) {

    jleAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

    btVector3 localInertia(0, 0, 0);

    const auto sizeX = glm::length(glm::vec3(worldMatrix[0])); // Basis vector X
    const auto sizeY = glm::length(glm::vec3(worldMatrix[1])); // Basis vector Y
    const auto sizeZ = glm::length(glm::vec3(worldMatrix[2])); // Basis vector Z

    auto v = btVector3{sizeX, sizeY, sizeZ};
    rigidbodyPtr->_optionalLocalShape = std::make_unique<btScaledBvhTriangleMeshShape>(reinterpret_cast<btBvhTriangleMeshShape *>(shape), v);
    shape = rigidbodyPtr->_optionalLocalShape.get();

    rigidbodyPtr->_motionState = std::make_unique<cRigidbodyMotionState>(&objectRef.getECS(), objectRef.objectIndex());

    btRigidBody::btRigidBodyConstructionInfo cInfo{
        rigidbodyPtr->_mass, rigidbodyPtr->_motionState.get(), shape, localInertia
    };

    btRigidBody* body = new btRigidBody(cInfo);

    body->setUserIndex(-1);
    body->setUserPointer(rigidbodyPtr);

    return body;
}
