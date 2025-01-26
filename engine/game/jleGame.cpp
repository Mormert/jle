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

#include "jleGame.h"

#include <jleGameEngine.h>
#include <game/jleGameRuntime.h>
#include <modules/graphics/core/jleFrameBufferInterface.h>
#include <modules/graphics/jleRenderThread.h>

jleGame::jleGame() = default;
jleGame::~jleGame() = default;

void
jleGame::update(jleEngineUpdateContext &ctx)
{
    _modules->update(ctx, *_ecs);
}

void
jleGame::start(jleSerializationContext& serializationContext) {}

void
jleGameModules::initialize(jlECS::ECS &ecs, jleSerializationContext& serializationContext)
{
    hierarchyModule->initializeECS(ecs);
    graphicsModule->initializeECS(ecs);
    physicsModule->initializeECS(ecs);

    luaModule->initializeModule(serializationContext);
    luaModule->initializeECS(ecs);
}

void
jleGameModules::update(jleEngineUpdateContext &ctx, jlECS::ECS& ecs)
{
    hierarchyModule->updateWorldMatrices(ecs);
    const std::vector<glm::mat4>& worldMatrices = hierarchyModule->getWorldMatrices();

    {
        jleGraphicsModule::UpdateContext graphicsUpdateContext{
            .in =    {  .screenX = ctx.gameRuntime.mainGameScreenFramebuffer->width(),
                        .screenY = ctx.gameRuntime.mainGameScreenFramebuffer->height(),
                        .worldMatrices = worldMatrices },
            .inOut = {  .ecs = ecs },
            .out =   {  .framePacket = ctx.currentFramePacket }};

        graphicsModule->update(graphicsUpdateContext);
    }

    {
        jlePhysicsModule::UpdateContext physicsUpdateContext{
            .in =     { .dt = 1.f / 60.f,
                        .worldMatrices = worldMatrices },
            .inOut =  { .ecs = ecs}
        };

        physicsModule->update(physicsUpdateContext);
    }

    {
        jleLuaModule::UpdateContext luaUpdateContext{
            .in =    { .dt = ctx.frameInfo.getDeltaTime() },
            .inOut = { .ecs = ecs }
        };

        luaModule->update(luaUpdateContext);
    }
}

void
jleGameModules::updateRenderablesOnly(jleEngineUpdateContext &ctx, jlECS::ECS& ecs)
{
    hierarchyModule->updateWorldMatrices(ecs);
    const std::vector<glm::mat4>& worldMatrices = hierarchyModule->getWorldMatrices();

    {
        jleGraphicsModule::UpdateContext graphicsUpdateContext{
            .in =    {  .screenX = ctx.gameRuntime.mainGameScreenFramebuffer->width(),
                   .screenY = ctx.gameRuntime.mainGameScreenFramebuffer->height(),
                   .worldMatrices = worldMatrices },
            .inOut = {  .ecs = ecs },
            .out =   {  .framePacket = ctx.currentFramePacket }};

        graphicsModule->update(graphicsUpdateContext);
    }
}

void
jleGameModules::populateSerializeableInterfaces(std::vector<jleSerializableInterface *> &interfaces)
{
    luaModule->populateSerializeableInterface(interfaces);
}
