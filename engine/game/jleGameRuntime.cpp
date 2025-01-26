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

#include "jleGameRuntime.h"

#include "jleEngineSettings.h"
#include "jleGame.h"
#include "jleGameEngine.h"
#include "jlECS/jlECS.h"

#include <modules/graphics/core/jleFramebufferScreen.h>
#include <modules/graphics/jleRenderThread.h>
#include <modules/input/jleInputModule.h>

#include <WickedEngine/wiJobSystem.h>

jleGameRuntime::jleGameRuntime(const jleGameConstructConfig &config)
{
    _gameConstructConfig = config;
    if(!_gameConstructConfig.ecsCreator){
        _gameConstructConfig.ecsCreator = [](){ return std::make_unique<jlECS::ECS>(); };
    }
}

void
jleGameRuntime::restartGame(jleSerializationContext& serializationContext)
{
    _gameIsGettingRestarted = true;
}

void
jleGameRuntime::killGame()
{
    _gameIsGettingKilled = true;
}

void
jleGameRuntime::haltGame()
{
    _gameHalted = true;
}

void
jleGameRuntime::unhaltGame()
{
    _gameHalted = false;
}

void
jleGameRuntime::executeNextFrame(jleEngineUpdateContext &ctx)
{
    LOG_VERBOSE << "Next frame dt: " << ctx.frameInfo.getDeltaTime();
    auto gameHaltedTemp = _gameHalted;
    _gameHalted = false;

    // Game thread
    wi::jobsystem::context jobsCtx;
    wi::jobsystem::Execute(jobsCtx, [&](wi::jobsystem::JobArgs args) { update(ctx); });

    // Render thread
    assert(false);
    // todo: fix that we feed the camera from the ecs cCamera here ..
    //JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS) { _engine.render(camera, ctx, jobsCtx); }
    _gameHalted = gameHaltedTemp;
}

bool
jleGameRuntime::isGameKilled() const
{
    if (_game) {
        return false;
    }
    return true;
}

bool
jleGameRuntime::isGameHalted() const
{
    return _gameHalted;
}

void
jleGameRuntime::update(jleEngineUpdateContext &ctx)
{
    ZoneScoped;

    if (!_gameHalted && _game) {
        _gameConstructConfig.modulesUpdate(getGame().getModules(), ctx, _game->getECS());
    }
}

void
jleGameRuntime::processGameReset(jleEngineUpdateContext &ctx)
{
    if (_gameIsGettingRestarted) {
        _game.reset();
        startGame(ctx.serializationContext);
        _gameIsGettingRestarted = false;
    }

    if (_gameIsGettingKilled) {
        _game.reset();
        _gameIsGettingKilled = false;
    }
}

jleGame &
jleGameRuntime::getGame()
{
    return *_game;
}

void
jleGameRuntime::startGame(jleSerializationContext& serializationContext)
{
    _game = std::make_unique<jleGame>();

    std::unique_ptr<jlECS::ECS> ecs = _gameConstructConfig.ecsCreator();

    constexpr bool gameRunning = true;
    std::unique_ptr<jleGameModules> modules = _gameConstructConfig.modulesCreator(gameRunning);
    _gameConstructConfig.modulesInitialize(*modules, *ecs, serializationContext);

    _game->_modules = std::move(modules);
    _game->_ecs = std::move(ecs);
}
