// Copyright (c) 2023. Johan Lind

#include "jleGameEngine.h"

#include "jleFramebufferScreen.h"
#include "jleFullscreenRendering.h"
#include "jleMouseInput.h"
#include "jleRendering.h"
#include "jleWindow.h"
#include "jleInput.h"
#include "jleTimerManager.h"
#include <plog/Log.h>

jleGameEngine::jleGameEngine(std::shared_ptr<jleGameSettings> gs) : gameSettings{gs}, jleCore{gs} { gEngine = this; }

jleGameEngine::~jleGameEngine() { gEngine = nullptr; }

void
jleGameEngine::startGame()
{
    if (!_gameCreator) {
        LOG_WARNING << "Game has not been set! Use SetGame<jleGameDerived>() "
                       "before starting the game.";
        return;
    }
    game = _gameCreator();
    game->start();
}

void
jleGameEngine::restartGame()
{
    game.reset();
    timerManager().clearTimers();
    startGame();
}

void
jleGameEngine::killGame()
{
    timerManager().clearTimers();
    game.reset();
}

void
jleGameEngine::haltGame()
{
    // TODO: Halt timers
    gameHalted = true;
}

void
jleGameEngine::unhaltGame()
{
    // TODO: Unhalt timers
    gameHalted = false;
}

void
jleGameEngine::executeNextFrame()
{
    LOG_VERBOSE << "Next frame dt: " << gCore->deltaFrameTime();
    auto gameHaltedTemp = gameHalted;
    gameHalted = false;
    update(deltaFrameTime());
    rendering().render(*mainScreenFramebuffer, gameRef().mainCamera);
    gameHalted = gameHaltedTemp;
}

bool
jleGameEngine::isGameKilled() const
{
    if (game) {
        return false;
    }
    return true;
}

bool
jleGameEngine::isGameHalted() const
{
    return gameHalted;
}

jleGame &
jleGameEngine::gameRef()
{
    return *game;
}

void
jleGameEngine::start()
{
    constexpr int initialScreenX = 1024;
    constexpr int initialScreenY = 1024;
    mainScreenFramebuffer = std::make_shared<jleFramebufferScreen>(initialScreenX, initialScreenY);

    const auto &mouse = gCore->input().mouse;
    mouse->setPixelatedScreenSize(initialScreenX, initialScreenY);
    mouse->setScreenSize(initialScreenX, initialScreenY);

    _fullscreen_renderer = std::make_unique<jleFullscreenRendering>();

#ifndef BUILD_EDITOR
    window().addWindowResizeCallback(
        std::bind(&jleGameEngine::gameWindowResizedEvent, this, std::placeholders::_1, std::placeholders::_2));
#endif

    LOG_INFO << "Starting the game engine";

    startGame();
}

void
jleGameEngine::resizeMainFramebuffer(unsigned int width, unsigned int height)
{
    mainScreenFramebuffer->resize(width, height);

    const auto &inputMouse = gCore->input().mouse;
    inputMouse->setPixelatedScreenSize(width, height);
    inputMouse->setScreenSize(width, height);
}

int
jleGameEngine::addGameWindowResizeCallback(std::function<void(unsigned int, unsigned int)> callback)
{
    unsigned int i = 0;

    // Find first available callback id
    for (auto it = gameWindowResizedCallbacks.cbegin(), end = gameWindowResizedCallbacks.cend();
         it != end && i == it->first;
         ++it, ++i) {
    }

    gameWindowResizedCallbacks.insert(
        std::make_pair(i, std::bind(callback, std::placeholders::_1, std::placeholders::_2)));

    return i;
}

void
jleGameEngine::removeGameWindowResizeCallback(unsigned int callbackId)
{
    gameWindowResizedCallbacks.erase(callbackId);
}

void
jleGameEngine::executeGameWindowResizedCallbacks(unsigned int w, unsigned int h)
{
    for (const auto &callback : gameWindowResizedCallbacks) {
        callback.second(w, h);
    }
}

void
jleGameEngine::gameWindowResizedEvent(unsigned int w, unsigned int h)
{
    executeGameWindowResizedCallbacks(w, h);
}

void
jleGameEngine::update(float dt)
{
    JLE_SCOPE_PROFILE_CPU(jleGameEngine_Update)
    if (!gameHalted && game) {
        game->update(dt);
        game->updateActiveScenes(dt);
    }
}

void
jleGameEngine::render()
{
    JLE_SCOPE_PROFILE_CPU(jleGameEngine_render)
    if (!gameHalted && game) {
        rendering().render(*mainScreenFramebuffer.get(), gameRef().mainCamera);
        rendering().clearBuffersForNextFrame();
        _fullscreen_renderer->renderFramebufferFullscreen(*mainScreenFramebuffer, window().width(), window().height());
    }
}

void
jleGameEngine::exiting()
{
    killGame();
}
