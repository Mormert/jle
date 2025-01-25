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
#include <modules/hierarchy/components/cTransform.h>

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
jleGraphicsModule::update(const jleGraphicsModule::UpdateContext &ctx)
{
    const std::vector<glm::mat4>& worldMatrices = ctx.in.worldMatrices;

    for (auto [objectIndex, camera] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cCamera>()) {
        cCamera::UpdateContext cameraUpdateCtx = {
            .in = {.transform = worldMatrices[objectIndex],
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
        light->ecsUpdate(ctx.out.framePacket, worldMatrices[objectIndex]);
    }

    for (auto [objectIndex, lightDirectional] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cLightDirectional>()) {
        lightDirectional->ecsUpdate(ctx.out.framePacket, worldMatrices[objectIndex]);
    }

    for (auto [objectIndex, mesh] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cMesh>()) {
        mesh->ecsUpdate(ctx.out.framePacket, worldMatrices[objectIndex], objectIndex);
    }

    for (auto [objectIndex, skinnedMesh] :
         ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cSkinnedMesh>()) {

        auto object = ctx.inOut.ecs.getObject(objectIndex);
        auto optionalAnimator = object.getComponentPtr<cAnimator>();

        skinnedMesh->ecsUpdate(ctx.out.framePacket, worldMatrices[objectIndex], optionalAnimator, objectIndex);
    }

    for (auto &skybox : ctx.inOut.ecs.iterate<cSkybox>()) {
        ctx.out.framePacket.settings.skybox = skybox.getSkyboxRef();
        // Break here so we only get one skybox
        break;
    }
}
