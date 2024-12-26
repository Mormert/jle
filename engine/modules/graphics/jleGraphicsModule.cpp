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
    for (auto [camera, transform] : ctx.inOut.ecs.iterateMulti<cCamera, cTransform>()) {
        cCamera::UpdateContext cameraUpdateCtx = {
            .in = {.transform = *transform,
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

    for (auto [light, transform] : ctx.inOut.ecs.iterateMulti<cLight, cTransform>()) {
        light->ecsUpdate(ctx.out.framePacket, *transform);
    }

    for (auto [lightDirectional, transform] : ctx.inOut.ecs.iterateMulti<cLightDirectional, cTransform>()) {
        lightDirectional->ecsUpdate(ctx.out.framePacket, *transform);
    }

    for (auto [objectIndex, mesh, transform] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cMesh, cTransform>()) {
        mesh->ecsUpdate(ctx.out.framePacket, *transform, objectIndex);
    }

    for (auto [objectIndex, skinnedMesh, transform] :
         ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cSkinnedMesh, cTransform>()) {

        auto object = ctx.inOut.ecs.getObject(objectIndex);
        auto optionalAnimator = object.getComponentPtr<cAnimator>();

        skinnedMesh->ecsUpdate(ctx.out.framePacket, *transform, optionalAnimator, objectIndex);
    }

    for (auto &skybox : ctx.inOut.ecs.iterate<cSkybox>()) {
        ctx.out.framePacket.settings.skybox = skybox.getSkyboxRef();
        // Break here so we only get one skybox
        break;
    }
}
