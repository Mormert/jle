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

#include "jlePhysics.h"
#include "jlePhysicsDebugDrawer.h"

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletDynamics/ConstraintSolver/btConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <LinearMath/btAlignedObjectArray.h>

#include "core/jleProfiler.h"
#include "components/cRigidbody.h"

jlePhysics::~jlePhysics() = default;

struct jlePhysics::jlePhysicsInternal
{
    btAlignedObjectArray<std::unique_ptr<btCollisionShape>> collisionShapes;

    std::unique_ptr<btBroadphaseInterface> broadphase;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btConstraintSolver> solver;
    std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
    std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;

    std::unique_ptr<jlePhysicsDebugDrawer> debugDraw;
};

jlePhysics::jlePhysics()
{
    _internal = std::make_unique<jlePhysicsInternal>();

    _internal->collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();

    _internal->dispatcher = std::make_unique<btCollisionDispatcher>(&*_internal->collisionConfiguration);

    _internal->broadphase = std::make_unique<btDbvtBroadphase>();

    _internal->solver = std::make_unique<btSequentialImpulseConstraintSolver>();

    _internal->dynamicsWorld =
        std::make_unique<btDiscreteDynamicsWorld>(&*_internal->dispatcher, &*_internal->broadphase, &*_internal->solver, &*_internal->collisionConfiguration);

    _internal->dynamicsWorld->setGravity(btVector3(0, 10, 0));

    _internal->debugDraw = std::make_unique<jlePhysicsDebugDrawer>();

    _internal->dynamicsWorld->setDebugDrawer(&*_internal->debugDraw);
}

void
jlePhysics::step(float dt)
{
    JLE_SCOPE_PROFILE_CPU(physics)

    _internal->dynamicsWorld->stepSimulation(dt);
}

void
jlePhysics::addRigidbody(btRigidBody *body)
{
    if (body)
        _internal->dynamicsWorld->addRigidBody(body);
}
void
jlePhysics::removeRigidbody(btRigidBody *body)
{
    if (body)
        _internal->dynamicsWorld->removeRigidBody(body);
}

void
jlePhysics::renderDebug(jleFramePacket &framePacket)
{
    if (renderDebugEnabled) {
        JLE_SCOPE_PROFILE_CPU(renderPhysicsDebug)
        _internal->debugDraw->setFramePacket(&framePacket);
        _internal->dynamicsWorld->debugDrawWorld();
    }
}

btDiscreteDynamicsWorld &
jlePhysics::dynamicsWorld()
{
    return *_internal->dynamicsWorld.get();
}
