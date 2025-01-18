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

#include "jleGraphicsModule.h"

#include "runtime/components/cCamera.h"
#include "runtime/components/cLight.h"
#include "runtime/components/cLightDirectional.h"
#include "runtime/components/cMesh.h"
#include "runtime/components/cSkinnedMesh.h"
#include "runtime/components/cSkybox.h"

#include <jlECS/jlECS.h>
#include <modules/core/components/cParent.h>
#include <modules/core/components/cTransform.h>

namespace {
    // Recursively compute and set each object's world transform in-place
    void computeWorldMatrixRecursive(const jlECS::ObjectRef &object,
                                     jlECS::ECS &ecs,
                                     std::vector<glm::mat4> &worldMatrices,
                                     std::vector<bool> &touchedTransforms)
    {
        const auto index = object.objectIndex();

        // If we've already computed this object's world transform, just return
        if (touchedTransforms[index]) {
            return;
        }

        const auto* transformComp = object.getComponentPtr<cTransform>();
        const glm::mat4 localMatrix = transformComp ? transformComp->getLocalMatrix() : glm::mat4(1.0f);

        const auto* parentComp = object.getComponentPtr<cParent>();
        if (!parentComp)
        {
            worldMatrices[index] = localMatrix;
            touchedTransforms[index] = true;
            return;
        }

        auto parentObj = parentComp->getParentRef(ecs);
        if (!touchedTransforms[parentObj.objectIndex()]) {
            computeWorldMatrixRecursive(parentObj, ecs, worldMatrices, touchedTransforms);
        }

        const glm::mat4 parentWorldMatrix = worldMatrices[parentObj.objectIndex()];
        const glm::mat4 finalWorldMatrix = parentWorldMatrix * localMatrix;

        worldMatrices[index] = finalWorldMatrix;
        touchedTransforms[index] = true;
    }

    // Gets the final world transforms from the local matrices stored in cTransforms
    std::vector<glm::mat4> getWorldTransforms(jlECS::ECS &ecs)
    {
        std::vector<glm::mat4> worldTransforms(ecs.allocatedObjectsCount());
        std::vector touchedTransforms(ecs.allocatedObjectsCount(), false);

        for (auto [objectIndex, transform] : ecs.iterateMulti_IncludeObjectIndex<cTransform>())
        {
            jlECS::ObjectRef object = ecs.getObject(objectIndex);
            computeWorldMatrixRecursive(object, ecs, worldTransforms, touchedTransforms);
        }

        return worldTransforms;
    }
}

void
jleGraphicsModule::initializeECS(jlECS::ECS &ecs)
{
    ecs.registerComponentType<cCamera>();
    ecs.registerComponentType<cLight>();
    ecs.registerComponentType<cLightDirectional>();
    ecs.registerComponentType<cMesh>();
    ecs.registerComponentType<cSkinnedMesh>();
    ecs.registerComponentType<cSkybox>();
}

void
jleGraphicsModule::update(jleGraphicsModule::UpdateContext &ctx)
{
    const auto worldTransforms = getWorldTransforms(ctx.inOut.ecs);

    for (auto [objectIndex, camera] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cCamera>()) {
        cCamera::UpdateContext cameraUpdateCtx = {
            .in = {.transform = worldTransforms[objectIndex],
                   .width = ctx.in.screenX,
                   .height = ctx.in.screenY},
            .out = {
                .camera = ctx.out.framePacket.camera
            }
        };
        camera->update(cameraUpdateCtx);

        // Break here, so we only get one camera
        break;
    }

    for (auto [objectIndex, light] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cLight>()) {
        light->ecsUpdate(ctx.out.framePacket, worldTransforms[objectIndex]);
    }

    for (auto [objectIndex, lightDirectional] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cLightDirectional>()) {
        lightDirectional->ecsUpdate(ctx.out.framePacket, worldTransforms[objectIndex]);
    }

    for (auto [objectIndex, mesh] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cMesh>()) {
        mesh->ecsUpdate(ctx.out.framePacket, worldTransforms[objectIndex], objectIndex);
    }

    for (auto [objectIndex, skinnedMesh] :
         ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cSkinnedMesh>()) {

        auto object = ctx.inOut.ecs.getObject(objectIndex);
        auto optionalAnimator = object.getComponentPtr<cAnimator>();

        skinnedMesh->ecsUpdate(ctx.out.framePacket, worldTransforms[objectIndex], optionalAnimator, objectIndex);
    }

    for (auto &skybox : ctx.inOut.ecs.iterate<cSkybox>()) {
        ctx.out.framePacket.settings.skybox = skybox.getSkyboxRef();
        // Break here so we only get one skybox
        break;
    }
}
