// Copyright (c) 2023. Johan Lind
#include "jlePhysics.h"
#include <glm/ext/matrix_transform.hpp>

#include "btBulletDynamicsCommon.h"

#include "cRigidbody.h"
#include "jleProfiler.h"

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

    // Update jle objects to new transforms
    for (int j = _dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--) {
        btCollisionObject *obj = _dynamicsWorld->getCollisionObjectArray()[j];
        btRigidBody *body = btRigidBody::upcast(obj);
        btTransform trans;
        if (body && body->getMotionState()) {
            body->getMotionState()->getWorldTransform(trans);
        } else {
            trans = obj->getWorldTransform();
        }
        cRigidbody *jleRigidbody = static_cast<cRigidbody *>(body->getUserPointer());
        if (jleRigidbody) {
            glm::mat4 matrix;
            trans.getOpenGLMatrix((btScalar*)&matrix);
            matrix = glm::scale(matrix, jleRigidbody->_size);
            jleRigidbody->getTransform().setWorldMatrix(matrix);
        }
    }
}

btRigidBody *
jlePhysics::createRigidbody(float mass,
                            const btTransform &startTransform,
                            btCollisionShape *shape,
                            cRigidbody *jleRigidbody)
{
    btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

    // Rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic) {
        shape->calculateLocalInertia(mass, localInertia);
    }

    btDefaultMotionState *myMotionState = new btDefaultMotionState(startTransform);

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

    btRigidBody *body = new btRigidBody(cInfo);

    body->setUserIndex(-1);
    body->setUserPointer(jleRigidbody);
    _dynamicsWorld->addRigidBody(body);
    return body;
}

void
jlePhysics::deleteRigidbody(btRigidBody *body)
{
    _dynamicsWorld->removeRigidBody(body);
    btMotionState *ms = body->getMotionState();
    delete body;
    delete ms;
}

void
jlePhysics::renderDebug()
{
    if(renderDebugEnabled)
    {
        JLE_SCOPE_PROFILE_CPU(renderPhysicsDebug)
        _dynamicsWorld->debugDrawWorld();
    }
}
