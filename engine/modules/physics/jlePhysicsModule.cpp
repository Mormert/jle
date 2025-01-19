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

#include "components/cRigidbody.h"
#include "modules/hierarchy/components/cTransform.h"

void
jlePhysicsModule::initializeECS(jlECS::ECS &ecs)
{
    ecs.registerComponentType<cRigidbody>({
        .onCreateCallback = [this](jlECS::CreateCallbackData& data) {
            onRigidbodyCreated(data);
        },
        .onDestroyCallback = [this](void* componentPtr) {
            onRigidbodyDestroyed(static_cast<cRigidbody*>(componentPtr));
        }
    });
}

void
jlePhysicsModule::update(jlePhysicsModule::UpdateContext &ctx)
{
    // Copy cTransform data into bullet
    for(auto [rb, transform] : ctx.inOut.ecs.iterateMulti<cRigidbody, cTransform>()){
        btTransform centerOfMassWorldTrans;
        centerOfMassWorldTrans.setFromOpenGLMatrix((btScalar *)&transform->getLocalMatrix()[0]);
        rb->getBody().setCenterOfMassTransform(centerOfMassWorldTrans);

        rb->_size.x = glm::length(glm::vec3(transform->getLocalMatrix()[0])); // Basis vector X
        rb->_size.y = glm::length(glm::vec3(transform->getLocalMatrix()[1])); // Basis vector Y
        rb->_size.z = glm::length(glm::vec3(transform->getLocalMatrix()[2])); // Basis vector Z
    }

    // Update bullet physics
    _physics->step(ctx.in.dt);

    // Copy transform results from bullet to cTransform
    for(auto [rb, transform] : ctx.inOut.ecs.iterateMulti<cRigidbody, cTransform>()){
        glm::mat4 matrix;
        rb->getBody().getWorldTransform().getOpenGLMatrix((btScalar *)&matrix);
        matrix = glm::scale(matrix, rb->_size);
        transform->setLocalMatrix(matrix);
    }
}

void
jlePhysicsModule::onRigidbodyCreated(jlECS::CreateCallbackData &createCallbackData)
{
    auto* rb = static_cast<cRigidbody*>(createCallbackData.componentPtr);
    jlECS::ObjectRef& objectRef = createCallbackData.objectRef;

    if(!objectRef.getComponent<cTransform>().isValid())
    {
        objectRef.addComponent<cTransform>();
    }

    if(!objectRef.getComponent<cMesh>().isValid())
    {
        objectRef.addComponent<cMesh>();
    }

    rb->setupRigidbody(_physics, *objectRef.getComponentPtr<cTransform>(), *objectRef.getComponentPtr<cMesh>());
}

void
jlePhysicsModule::onRigidbodyDestroyed(cRigidbody *rb)
{
    _physics->removeRigidbody(&rb->getBody());
}

void jlePhysicsModule::onRigidbodyCopied(cRigidbody *source, cRigidbody *dest) {
    assert(false); // todo
}
