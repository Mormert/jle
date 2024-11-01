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

#include <modules/jleEngineModulesContext.h>
#include <random>

#include "runtime/components/cCamera.h"
#include "runtime/components/cCameraFPV.h"
#include "runtime/components/cLight.h"
#include "runtime/components/cLightDirectional.h"
#include "runtime/components/cMesh.h"
#include "runtime/components/cSkinnedMesh.h"
#include "runtime/components/cSkybox.h"

#include "jlECS/jlECS.h"

void
jleGraphicsModule::initializeECS(jlECS::ECS &ecs)
{
    ecs.registerComponentType<cCamera>();
    ecs.registerComponentType<cCameraFPV>();
    ecs.registerComponentType<cLight>();
    ecs.registerComponentType<cLightDirectional>();
    ecs.registerComponentType<cMesh>();
    ecs.registerComponentType<cSkinnedMesh>();
    ecs.registerComponentType<cSkybox>();
}

void
jleGraphicsModule::update(jleGraphicsModule::UpdateContext &ctx)
{
    for (auto &camera : ctx.inOut.ecs.iterate<cCamera>()) {
        camera.ecsUpdate(ctx.out.camera, ctx.in.screenX, ctx.in.screenY);
    }

    // for (auto &cameraFPV : ecs.iterate<cCameraFPV>()) {
    //     cameraFPV.ecsUpdate(ctx);
    // }

    for (auto &light : ctx.inOut.ecs.iterate<cLight>()) {
        light.ecsUpdate(ctx.out.framePacket);
    }

    for (auto &lightDirectional : ctx.inOut.ecs.iterate<cLightDirectional>()) {
        lightDirectional.ecsUpdate(ctx.out.framePacket);
    }

    for (auto &mesh : ctx.inOut.ecs.iterate<cMesh>()) {
        mesh.ecsUpdate(ctx.out.framePacket);
    }

    for (auto &skinnedMesh : ctx.inOut.ecs.iterate<cSkinnedMesh>()) {
        skinnedMesh.ecsUpdate(ctx.out.framePacket);
    }

    for (auto &skybox : ctx.inOut.ecs.iterate<cSkybox>()) {
        // skybox.update(ctx);
    }
}
