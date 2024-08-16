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

#include "core/jleCommon.h"

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
class jle3DGraph;

class jlePhysics
{
public:
    jlePhysics();

    void step(float dt);

    void addRigidbody(btRigidBody *body);

    void removeRigidbody(btRigidBody *body);

    bool renderDebugEnabled = false;

    void renderDebug(jle3DGraph& graph);

    btDiscreteDynamicsWorld& dynamicsWorld();

private:
    btAlignedObjectArray<std::unique_ptr<btCollisionShape>> _collisionShapes;

    std::unique_ptr<btBroadphaseInterface> _broadphase;
    std::unique_ptr<btCollisionDispatcher> _dispatcher;
    std::unique_ptr<btConstraintSolver> _solver;
    std::unique_ptr<btDefaultCollisionConfiguration> _collisionConfiguration;
    std::unique_ptr<btDiscreteDynamicsWorld> _dynamicsWorld;

    std::unique_ptr<jlePhysicsDebugDrawer> _debugDraw;
};

#endif // JLE_PHYSICS_H
