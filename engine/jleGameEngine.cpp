// Copyright (c) 2022. Johan Lind

#include "jleGameEngine.h"

#include "jleExplicitInclude.h"
#include "jleFrameBuffer.h"
#include "jleFullscreenRendering.h"
#include "jleMouseInput.h"
#include "jleRendering.h"
#include "jleWindow.h"
#include <plog/Log.h>

jleGameEngine::jleGameEngine(std::shared_ptr<jleGameSettings> gs)
    : gameSettings{gs}, jleCore{gs} {
    gEngine = this;
    SetGameDimsPixels(gs->framebufferSettings.fixedAxis,
                      gs->framebufferSettings.fixedAxisPixels);
}

jleGameEngine::~jleGameEngine() { gEngine = nullptr; }

void jleGameEngine::SetGameDimsPixels(FIXED_AXIS fa, unsigned int pixels) {
    fixed_axis = fa;
    gameDimsPixels = pixels;
}

std::pair<unsigned int, unsigned int> jleGameEngine::framebufferDimensions(
    unsigned int windowWidth, unsigned int windowHeight) {
    if (fixed_axis == FIXED_AXIS::height) {
        float aspect =
            static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

        unsigned int w = static_cast<unsigned int>(gameDimsPixels * aspect);
        return std::make_pair(w, gameDimsPixels);
    }
    else if (fixed_axis == FIXED_AXIS::width) {
        float aspect =
            static_cast<float>(windowHeight) / static_cast<float>(windowWidth);

        unsigned int h = static_cast<unsigned int>(gameDimsPixels * aspect);
        return std::make_pair(gameDimsPixels, h);
    }

    return std::make_pair(windowWidth, windowHeight);
}

void jleGameEngine::startGame() {
    if (!_gameCreator) {
        LOG_WARNING << "Game has not been set! Use SetGame<jleGameDerived>() "
                       "before starting the game.";
        return;
    }
    game = _gameCreator();
    game->start();

    game->_mainCamera._cameraWidth = framebuffer_main->width();
    game->_mainCamera._cameraHeight = framebuffer_main->height();
}

void jleGameEngine::restartGame() {
    game.reset();
    _timerManager.clearTimers();
    startGame();
}

void jleGameEngine::killGame() {
    _timerManager.clearTimers();
    game.reset();
}

void jleGameEngine::haltGame() {
    // TODO: Halt timers
    gameHalted = true;
}

void jleGameEngine::unhaltGame() {
    // TODO: Unhalt timers
    gameHalted = false;
}

void jleGameEngine::executeNextFrame() {
    LOG_VERBOSE << "Next frame dt: " << status->deltaFrameTime();
    auto gameHaltedTemp = gameHalted;
    gameHalted = false;
    update(status->deltaFrameTime());
    rendering->render(*framebuffer_main, gameRef()._mainCamera);
    gameHalted = gameHaltedTemp;
}

bool jleGameEngine::isGameKilled() {
    if (game) {
        return false;
    }
    return true;
}

bool jleGameEngine::isGameHalted() { return gameHalted; }

jleGame& jleGameEngine::gameRef() { return *game; }

void jleGameEngine::start() {
    auto dims = framebufferDimensions(core_settings->windowSettings.width,
                                      core_settings->windowSettings.height);
    framebuffer_main =
        std::make_shared<jleFramebuffer>(dims.first, dims.second);

    const auto& internalInputMouse = jleCore::core->input->mouse;
    internalInputMouse->pixelatedScreenSize(dims.first, dims.second);
    internalInputMouse->screenSize(core_settings->windowSettings.width,
                                   core_settings->windowSettings.height);

    _fullscreen_renderer = std::make_unique<jleFullscreenRendering>();

    window->addWindowResizeCallback(
        std::bind(&jleGameEngine::framebufferResizeEvent,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2));

    // framebuffer_main->resizeFramebuffer(200, 200);

    LOG_INFO << "Starting the game engine";

    startGame();
}

void jleGameEngine::framebufferResizeEvent(unsigned int width,
                                           unsigned int height) {
    auto dims = framebufferDimensions(width, height);
    framebuffer_main->resize(dims.first, dims.second);

    const auto& internalInputMouse =
        std::static_pointer_cast<jleMouseInput>(jleCore::core->input->mouse);
    internalInputMouse->pixelatedScreenSize(dims.first, dims.second);
    internalInputMouse->screenSize(width, height);

    game->_mainCamera._cameraWidth = dims.first;
    game->_mainCamera._cameraHeight = dims.second;
}

void jleGameEngine::update(float dt) {
    JLE_SCOPE_PROFILE(jleGameEngine::Update)
    if (!gameHalted && game) {
        game->update(dt);
        game->updateActiveScenes(dt);
    }
}

void jleGameEngine::render() {
    JLE_SCOPE_PROFILE(jleGameEngine::Render)
    if (!gameHalted && game) {
        rendering->render(*framebuffer_main.get(), gameRef()._mainCamera);
        rendering->clearBuffersForNextFrame();
        _fullscreen_renderer->renderFramebufferFullscreen(
            *framebuffer_main.get(),
            window->GetWindowWidth(),
            window->GetWindowHeight());
    }
}

void jleGameEngine::exiting() { killGame(); }
