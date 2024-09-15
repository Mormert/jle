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
#include <glm/ext/matrix_transform.hpp>

#include "btBulletDynamicsCommon.h"

#include "jleProfiler.h"
#include "modules/physics/components/cRigidbody.h"

jlePhysics::jlePhysics()
{
    _collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();

    _dispatcher = std::make_unique<btCollisionDispatcher>(&*_collisionConfiguration);

    _broadphase = std::make_unique<btDbvtBroadphase>();

    _solver = std::make_unique<btSequentialImpulseConstraintSolver>();

    _dynamicsWorld =
        std::make_unique<btDiscreteDynamicsWorld>(&*_dispatcher, &*_broadphase, &*_solver, &*_collisionConfiguration);

    _dynamicsWorld->setGravity(btVector3(0, 10, 0));

    _debugDraw = std::make_unique<jlePhysicsDebugDrawer>();

    _dynamicsWorld->setDebugDrawer(&*_debugDraw);
}

void
jlePhysics::step(float dt)
{
    JLE_SCOPE_PROFILE_CPU(physics)

    _dynamicsWorld->stepSimulation(dt);
}

void
jlePhysics::addRigidbody(btRigidBody *body)
{
    if (body)
        _dynamicsWorld->addRigidBody(body);
}
void
jlePhysics::removeRigidbody(btRigidBody *body)
{
    if (body)
        _dynamicsWorld->removeRigidBody(body);
}

void
jlePhysics::renderDebug(jleFramePacket &framePacket)
{
    if (renderDebugEnabled) {
        JLE_SCOPE_PROFILE_CPU(renderPhysicsDebug)
        _debugDraw->setFramePacket(&framePacket);
        _dynamicsWorld->debugDrawWorld();
    }
}

btDiscreteDynamicsWorld &
jlePhysics::dynamicsWorld()
{
    return *_dynamicsWorld.get();
}
