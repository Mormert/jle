// Copyright (c) 2023. Johan Lind

#include "cRigidbody.h"
#include <BulletCollision/CollisionShapes/btBoxShape.h>

#include "jlePhysics.h"

#include "jleGameEngine.h"

cRigidbody::cRigidbody(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void
cRigidbody::editorUpdate(float dt)
{

}

void
cRigidbody::start()
{
    btTransform transform;
    transform.setFromOpenGLMatrix((btScalar*)&getTransform().getWorldMatrix());

    btCollisionShape* shape = new btBoxShape(btVector3(btScalar(5.), btScalar(5.), btScalar(5.)));

    gEngine->physics().createRigidbody(1.f, transform, shape, this);
}

void
cRigidbody::update(float dt)
{

}
