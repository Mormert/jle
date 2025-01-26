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
    _modules->hierarchyModule->updateWorldMatrices(*_ecs);
    const std::vector<glm::mat4>& worldMatrices = _modules->hierarchyModule->getWorldMatrices();

    {
        jleGraphicsModule::UpdateContext graphicsUpdateContext{
            .in = {  .screenX = ctx.gameRuntime.mainGameScreenFramebuffer->width(),
                        .screenY = ctx.gameRuntime.mainGameScreenFramebuffer->height(),
                        .worldMatrices = worldMatrices   },
            .inOut = {.ecs = *_ecs},
            .out = {.framePacket = ctx.currentFramePacket}};

        _modules->graphicsModule->update(graphicsUpdateContext);
    }

    {
        jlePhysicsModule::UpdateContext physicsUpdateContext{
            .in = {  .dt = 1.f / 60.f,
                        .worldMatrices = worldMatrices   },
            .inOut = { .ecs = *_ecs}
        };

        _modules->physicsModule->update(physicsUpdateContext);
    }

    {
        jleLuaModule::UpdateContext luaUpdateContext{
            .in = {.dt = ctx.frameInfo.getDeltaTime()},
            .inOut = {.ecs = *_ecs}
        };

        _modules->luaModule->update(luaUpdateContext);
    }
}

void
jleGame::start(GameStartContext& ctx)
{
    _ecs = std::move(ctx.ecs);

    _modules->hierarchyModule->initializeECS(*_ecs);
    _modules->graphicsModule->initializeECS(*_ecs);
    _modules->physicsModule->initializeECS(*_ecs);

    _modules->luaModule->initializeModule(ctx.serializationContext);
    _modules->luaModule->initializeECS(*_ecs);
}

