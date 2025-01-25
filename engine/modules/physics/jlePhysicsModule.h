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

#pragma once

#include <glm/fwd.hpp>
#include <jlECS/jlECS.h>

#include "jlePhysics.h"

class btTriangleMesh;
class btCollisionShape;
class jleMesh;
class btBvhTriangleMeshShape;
class btConvexHullShape;

namespace jlECS
{
class ECS;
class ObjectRef;
struct CreateComponentData;
}

class cTransform;
class cMesh;
class cRigidbody;

class jlePhysicsModule
{
public:
    jlePhysicsModule();
    ~jlePhysicsModule();

    virtual void initializeECS(jlECS::ECS &ecs);

    struct UpdateContext {
        struct In {
            float dt = 1.f / 60.f;
            const std::vector<glm::mat4>& worldMatrices;
        } in;

        struct InOut {
            jlECS::ECS &ecs;
        } inOut;
    };

    void update(const UpdateContext &ctx);

    [[nodiscard]] jlePhysics& getPhysics() const { return *_physics; }

protected:
    void onRigidbodyCreated(const jlECS::CreateComponentData &createCallbackData);
    void onRigidbodyDestroyed(const jlECS::DestroyComponentData &destroyCallbackData);
    void onRigidbodyCopied(cRigidbody *source, cRigidbody *dest);

    void setupRigidbody(jlECS::ObjectRef& objectRef, cRigidbody *rigidbodyPtr, const glm::mat4& worldMatrix, cMesh *meshOptional);

    void updateRigidbodyScaling(jlECS::ObjectRef& objectRef, cRigidbody *rigidbodyPtr, const glm::mat4& worldMatrix, cMesh *meshOptional);

    std::unique_ptr<jlePhysics> _physics;

    std::vector<jlECS::CreateComponentData> _rigidbodyCreateRequests;
    std::vector<btRigidBody*> _rigidbodyDestroyRequests;

    std::shared_ptr<btConvexHullShape> getDynamicConvexShape(const std::shared_ptr<jleMesh>& meshPtr);
    std::shared_ptr<btBvhTriangleMeshShape> getStaticConcaveShape(const std::shared_ptr<jleMesh>& meshPtr);

    std::unordered_map<std::shared_ptr<jleMesh>, std::shared_ptr<btConvexHullShape>> _dynamicConvexShapes;

    std::unordered_map<std::shared_ptr<jleMesh>, std::shared_ptr<btTriangleMesh>> _staticConcaveShapeMeshInterfaces;
    std::unordered_map<std::shared_ptr<jleMesh>, std::shared_ptr<btBvhTriangleMeshShape>> _staticConcaveShapes;

private:
    btRigidBody* createDynamicRigidbody(jlECS::ObjectRef& objectRef, cRigidbody *rigidbodyPtr, const glm::mat4 &worldMatrix,btCollisionShape *shape);
    btRigidBody* createStaticRigidbody(jlECS::ObjectRef& objectRef, cRigidbody *rigidbodyPtr, const glm::mat4& worldMatrix, btCollisionShape *shape);
};
