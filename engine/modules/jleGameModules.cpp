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

#include "jleGameModules.h"

#include "game/jleGameRuntime.h"
#include "graphics/core/jleFrameBufferInterface.h"
#include "jleEngineUpdateContext.h"
#include "jleGameEngine.h"

#include <modules/graphics/jleGraphicsModule.h>
#include <modules/hierarchy/jleHierarchyModule.h>
#include <modules/physics/jlePhysicsModule.h>
#include <modules/scripting/jleLuaModule.h>

void
jleGameModules::initialize(jlECS::ECS &ecs, jleSerializationContext& serializationContext)
{
    auto* hierarchyModule = getModule<jleHierarchyModule>();
    auto* graphicsModule = getModule<jleGraphicsModule>();
    auto* physicsModule = getModule<jlePhysicsModule>();
    auto* luaModule = getModule<jleLuaModule>();

    assert(hierarchyModule && graphicsModule && physicsModule && luaModule);

    hierarchyModule->initializeECS(ecs);

    graphicsModule->initializeECS(ecs);

    physicsModule->initializeECS(ecs);

    luaModule->initializeModule(serializationContext);
    luaModule->initializeECS(ecs);
}

void
jleGameModules::update(jleEngineUpdateContext &ctx, jlECS::ECS& ecs)
{
    auto* hierarchyModule = getModule<jleHierarchyModule>();
    auto* graphicsModule = getModule<jleGraphicsModule>();
    auto* physicsModule = getModule<jlePhysicsModule>();
    auto* luaModule = getModule<jleLuaModule>();

    assert(hierarchyModule && graphicsModule && physicsModule && luaModule);

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
    auto* hierarchyModule = getModule<jleHierarchyModule>();
    auto* graphicsModule = getModule<jleGraphicsModule>();

    assert(hierarchyModule && graphicsModule);

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
    auto* luaModule = getModule<jleLuaModule>();

    assert(luaModule);

    luaModule->populateSerializeableInterface(interfaces);
}
