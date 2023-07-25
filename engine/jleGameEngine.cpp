// Copyright (c) 2023. Johan Lind

#include "jleGameEngine.h"

#include "jleFramebufferScreen.h"
#include "jleFullscreenRendering.h"
#include "jleInput.h"
#include "jleMouseInput.h"
#include "jlePhysics.h"
#include "jleRendering.h"
#include "jleTimerManager.h"
#include "jleWindow.h"
#include <plog/Log.h>

#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#include <RmlUi_Backend.h>
#include <shell/include/Shell.h>

jleGameEngine::jleGameEngine() : jleCore()
{
    gEngine = this;

    LOG_INFO << "Starting the lua environment";
    _luaEnvironment = std::make_unique<jleLuaEnvironment>();
}

jleGameEngine::~jleGameEngine() { gEngine = nullptr; }

void
jleGameEngine::startGame()
{
    if (!_gameCreator) {
        LOG_WARNING << "Game has not been set! Use SetGame<jleGameDerived>() "
                       "before starting the game.";
        return;
    }

    if (!_physics) {
        // Re-initialize the physics
        _physics = std::make_unique<jlePhysics>();
    }

    game = _gameCreator();
    game->start();
}

void
jleGameEngine::restartGame()
{
    game.reset();
    _physics.reset();

    timerManager().clearTimers();
    startGame();
}

void
jleGameEngine::killGame()
{
    timerManager().clearTimers();
    game.reset();
    _physics.reset();
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

jlePhysics &
jleGameEngine::physics()
{
    return *_physics;
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
jleGameEngine::startRmlUi()
{

    auto width = mainScreenFramebuffer->width();
    auto height = mainScreenFramebuffer->height();

    if (!Shell::Initialize()) {
        LOGE << "Failed to init Shell for RmlUi";
        return;
    }

    // Constructs the system and render interfaces, creates a window, and attaches the renderer.
    if (!Backend::Initialize("RmlUiWindow", width, height, true)) {
        LOGE << "Failed to init backend for RmlUi";
        return;
    }

    // Install the custom interfaces constructed by the backend before initializing RmlUi.
    Rml::SetSystemInterface(Backend::GetSystemInterface());
    Rml::SetRenderInterface(Backend::GetRenderInterface());

    // RmlUi initialisation.
    Rml::Initialise();

    Rml::Log::Message(Rml::Log::LT_WARNING, "Test warning.");

    context = Rml::CreateContext("main", Rml::Vector2i(width, height));
    if (!context) {
        Rml::Shutdown();
        Backend::Shutdown();
        Shell::Shutdown();
        LOGE << "Failed to init backend for RmlUi";
        return;
    }

    Rml::Debugger::Initialise(context);

    Shell::LoadFonts();

    Rml::LoadFontFace("C:/dev/cgfx/cgfx/GameResources/LatoLatin-Regular.ttf");
    Rml::LoadFontFace("C:/dev/cgfx/cgfx/GameResources/LatoLatin-Bold.ttf");
    Rml::LoadFontFace("C:/dev/cgfx/cgfx/GameResources/LatoLatin-Italic.ttf");
    Rml::LoadFontFace("C:/dev/cgfx/cgfx/GameResources/LatoLatin-BoldItalic.ttf");

    if (auto doc = context->LoadDocument("assets/demo.rml")) {
        doc->Show();
    }

    Rml::Debugger::SetVisible(true);
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

    luaEnvironment()->loadScript("ER:/scripts/engine.lua");

    startRmlUi();

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
        {
            JLE_SCOPE_PROFILE_CPU(jleGameEngine_updateGame)
            game->update(dt);
        }
        {
            JLE_SCOPE_PROFILE_CPU(jleGameEngine_updateActiveScenes)
            game->updateActiveScenes(dt);
        }
        {
            JLE_SCOPE_PROFILE_CPU(RmlUi)
            context->Update();
        }
        physics().step(dt);
    }
}

void
jleGameEngine::render()
{
    JLE_SCOPE_PROFILE_CPU(jleGameEngine_render)
    if (!gameHalted && game) {

        // Render to game view
        static jleFramebufferMultisample msaa{mainScreenFramebuffer->width(), mainScreenFramebuffer->height(), 4};

        if (mainScreenFramebuffer->width() != msaa.width() || mainScreenFramebuffer->height() != msaa.height()) {
            msaa.resize(mainScreenFramebuffer->width(), mainScreenFramebuffer->height());
        }

        rendering().renderMSAA(*mainScreenFramebuffer.get(), msaa, gameRef().mainCamera);
        rendering().clearBuffersForNextFrame();
        _fullscreen_renderer->renderFramebufferFullscreen(*mainScreenFramebuffer, window().width(), window().height());
    }
}

void
jleGameEngine::exiting()
{
    killGame();
    killRmlUi();
}

void
jleGameEngine::killRmlUi()
{
    Rml::Shutdown();
    Backend::Shutdown();
    Shell::Shutdown();
}

std::shared_ptr<jleLuaEnvironment> &
jleGameEngine::luaEnvironment()
{
    return _luaEnvironment;
}
