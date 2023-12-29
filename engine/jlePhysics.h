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

#ifndef JLE_PHYSICS_H
#define JLE_PHYSICS_H

#include <BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletDynamics/ConstraintSolver/btConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <LinearMath/btAlignedObjectArray.h>

#include "jlePhysicsDebugDrawer.h"

#include <memory>

class cRigidbody;

class jlePhysics
{
public:
    jlePhysics();

    void step(float dt);

    btRigidBody* createRigidbody(float mass, const btTransform& startTransform, btCollisionShape* shape, cRigidbody* jleRigidbody);

    void deleteRigidbody(btRigidBody* body);

    bool renderDebugEnabled = true;

    void renderDebug();

private:
    btAlignedObjectArray<std::unique_ptr<btCollisionShape>> _collisionShapes;

    std::unique_ptr<btBroadphaseInterface> _broadphase;
    std::unique_ptr<btCollisionDispatcher> _dispatcher;
    std::unique_ptr<btConstraintSolver> _solver;
    std::unique_ptr<btDefaultCollisionConfiguration> _collisionConfiguration;
    std::unique_ptr<btDiscreteDynamicsWorld> _dynamicsWorld;

    std::unique_ptr<btIDebugDraw> _debugDraw;
};

#endif // JLE_PHYSICS_H
