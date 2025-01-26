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

#include "core/jleFramebufferScreen.h"
#include "jleFramePacket.h"
#include "jleGraphics.h"

#include "runtime/components/cCamera.h"
#include "runtime/components/cLight.h"
#include "runtime/components/cLightDirectional.h"
#include "runtime/components/cMesh.h"
#include "runtime/components/cSkinnedMesh.h"
#include "runtime/components/cSkybox.h"
#include <modules/hierarchy/components/cTransform.h>

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
jleGraphicsModule::initializeModule(jleSerializationContext &ctx)
{
    _renderThread = std::make_unique<jleRenderThread>();
    ctx.serializationInterfaces.push_back(_renderThread.get());

    _graphics = std::make_unique<jleGraphics>(ctx);
    _fullscreen_renderer = std::make_unique<jleFullscreenRendering>(ctx);

    constexpr int initialScreenX = 1024;
    constexpr int initialScreenY = 1024;
    _screenFramebuffer = std::make_unique<jleFramebufferScreen>(initialScreenX, initialScreenY);
    _msaaFramebuffer = std::make_unique<jleFramebufferMultisample>(initialScreenX, initialScreenY, 4);
}
void
jleGraphicsModule::preRender()
{
    getCurrentFramePacket().emptyQueues();
}

void
jleGraphicsModule::render(int windowX, int windowY)
{
    ZoneScoped;

    // Check that the module is initialized
    if (!_renderThread) {
        return;
    }

    if (_screenFramebuffer->width() != windowX || _screenFramebuffer->height() != windowY) {
        _screenFramebuffer->resize(windowX, windowY);
        _msaaFramebuffer->resize(windowX, windowY);
    }

    _renderThread->processRenderQueue();

    _graphics->render(*_msaaFramebuffer, getPreviousFramePacket());
    _msaaFramebuffer->blitToOther(*_screenFramebuffer);
    _fullscreen_renderer->renderFramebufferFullscreen(*_screenFramebuffer, windowX, windowY);
}

void
jleGraphicsModule::postRender()
{
    _currentFramePacketIndex = (_currentFramePacketIndex + 1) % 2;
}

jleGraphicsModule::~jleGraphicsModule() = default;

void
jleGraphicsModule::populateSerializeableInterface(std::vector<jleSerializableInterface *> &interfaces)
{
    if (jleRenderThread *renderThread = _renderThread.get()) {
        interfaces.push_back(renderThread);
    }
}

void
jleGraphicsModule::update(const jleGraphicsModule::UpdateContext &ctx)
{
    const std::vector<glm::mat4>& worldMatrices = ctx.in.worldMatrices;

    auto& framePacket = getCurrentFramePacket();

    for (auto [objectIndex, camera] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cCamera>()) {
        cCamera::UpdateContext cameraUpdateCtx = {
            .in = {.transform = worldMatrices[objectIndex],
                   .width = ctx.in.screenX,
                   .height = ctx.in.screenY},
            .out = {
                .camera = framePacket.camera
            }
        };
        camera->update(cameraUpdateCtx);

        // Break here, so we only get one camera
        break;
    }

    for (auto [objectIndex, light] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cLight>()) {
        light->ecsUpdate(framePacket, worldMatrices[objectIndex]);
    }

    for (auto [objectIndex, lightDirectional] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cLightDirectional>()) {
        lightDirectional->ecsUpdate(framePacket, worldMatrices[objectIndex]);
    }

    for (auto [objectIndex, mesh] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cMesh>()) {
        mesh->ecsUpdate(framePacket, worldMatrices[objectIndex], objectIndex);
    }

    for (auto [objectIndex, skinnedMesh] :
         ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cSkinnedMesh>()) {

        auto object = ctx.inOut.ecs.getObject(objectIndex);
        auto optionalAnimator = object.getComponentPtr<cAnimator>();

        skinnedMesh->ecsUpdate(framePacket, worldMatrices[objectIndex], optionalAnimator, objectIndex);
    }

    for (auto &skybox : ctx.inOut.ecs.iterate<cSkybox>()) {
        framePacket.settings.skybox = skybox.getSkyboxRef();
        // Break here so we only get one skybox
        break;
    }
}
