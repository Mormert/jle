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

#include <game/jleGameRuntime.h>
#include <modules/graphics/core/jleFrameBufferInterface.h>
#include <modules/graphics/jleRenderThread.h>

jleGame::jleGame() = default;
jleGame::~jleGame() = default;

void
jleGame::injectModules(std::unique_ptr<jleGameModules> modules)
{
    _modules = std::move(modules);
}

void
jleGame::update(jleEngineUpdateContext &ctx)
{
    jleGraphicsModule::UpdateContext graphicsUpdateContext{
        .in = {.screenX = ctx.gameRuntime.mainGameScreenFramebuffer->width(),
               .screenY = ctx.gameRuntime.mainGameScreenFramebuffer->height()},
        .inOut = {.ecs = *_gameState.ecs},
        .out = {.framePacket = ctx.currentFramePacket}};

    _modules->graphicsModule->update(graphicsUpdateContext);
}

void
jleGame::start(GameStartContext& ctx)
{
    _gameState.ecs = std::move(ctx.ecs);
    auto& ecs = *_gameState.ecs;

    _modules->coreModule->initializeECS(ecs);
    _modules->graphicsModule->initializeECS(ecs);
    _modules->physicsModule->initializeECS(ecs);
}

