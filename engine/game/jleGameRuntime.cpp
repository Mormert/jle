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

#include "core/jleTimerManager.h"
#include "jleEngineSettings.h"
#include "jleGame.h"
#include "jleGameEngine.h"
#include "jlECS/jlECS.h"

#include <modules/graphics/jleRenderThread.h>
#include <modules/input/jleInput.h>
#include <modules/graphics/core/jleFramebufferScreen.h>

#include <WickedEngine/wiJobSystem.h>

jleGameRuntime::jleGameRuntime(const jleGameConstructConfig &config, jleGameEngine& engine) : _engine(engine)
{
    _gameConstructConfig = config;
    if(!_gameConstructConfig.ecsCreator){
        _gameConstructConfig.ecsCreator = [](){ return std::make_unique<jlECS::ECS>(); };
    }

    _timerManager = std::make_unique<jleTimerManager>();

    constexpr int initialScreenX = 1024;
    constexpr int initialScreenY = 1024;
    mainGameScreenFramebuffer = std::make_unique<jleFramebufferScreen>(initialScreenX, initialScreenY);

    addGameWindowResizeCallback([this](auto &&PH1, auto &&PH2) {
        auto updateContext = _engine.createUpdateContext();
        _engine._gameRuntime->resizeMainFramebuffer(
            updateContext, std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    });
}

jleTimerManager &
jleGameRuntime::timerManager()
{
    return *_timerManager;
}

void
jleGameRuntime::restartGame(jleSerializationContext& serializationContext)
{
    _game.reset();

    _timerManager->clearTimers();

    startGame(serializationContext);
}

void
jleGameRuntime::killGame()
{
    _timerManager->clearTimers();
    _game.reset();
}

void
jleGameRuntime::haltGame()
{
    // TODO: Halt timers
    _gameHalted = true;
}

void
jleGameRuntime::unhaltGame()
{
    // TODO: Unhalt timers
    _gameHalted = false;
}

void
jleGameRuntime::executeNextFrame(jleEngineUpdateContext &ctx)
{
    LOG_VERBOSE << "Next frame dt: " << ctx.frameInfo.getDeltaTime();
    auto gameHaltedTemp = _gameHalted;
    _gameHalted = false;

    //
   // jleCamera camera = getGame();

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
jleGameRuntime::resizeMainFramebuffer(jleEngineUpdateContext &ctx, unsigned int width, unsigned int height)
{
    ctx.renderThread.runOnRenderThread([this, width, height]() { mainGameScreenFramebuffer->resize(width, height); });

    auto &inputMouse = ctx.inputModule.mouse;
    inputMouse.setScreenSize(width, height);
}

void
jleGameRuntime::update(jleEngineUpdateContext &ctx)
{
    ZoneScoped;

    if (!_gameHalted && _game) {
        _timerManager->process();

        _game->update(ctx);

        // rmlContext_notUsed->Update();
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
    jleGame::GameStartContext startContext{
        .serializationContext = serializationContext,
        .ecs = _gameConstructConfig.ecsCreator()
    };

    _game = _gameConstructConfig.gameCreator();
    std::unique_ptr<jleGameModules> modules = _gameConstructConfig.modulesCreator();
    _game->injectModules(std::move(modules));
    _game->start(startContext);
}

void
jleGameRuntime::gameWindowResizedEvent(unsigned int w, unsigned int h)
{
    for (const auto &callback : _gameWindowResizedCallbacks) {
        callback.second(w, h);
    }
}

int
jleGameRuntime::addGameWindowResizeCallback(std::function<void(unsigned int, unsigned int)> callback)
{
    unsigned int i = 0;

    // Find first available callback id
    for (auto it = _gameWindowResizedCallbacks.cbegin(), end = _gameWindowResizedCallbacks.cend();
         it != end && i == it->first;
         ++it, ++i) {
    }

    _gameWindowResizedCallbacks.insert(
        std::make_pair(i, std::bind(callback, std::placeholders::_1, std::placeholders::_2)));

    return i;
}

void
jleGameRuntime::removeGameWindowResizeCallback(unsigned int callbackId)
{
    _gameWindowResizedCallbacks.erase(callbackId);
}
