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

#include "jleGameEngine.h"
#include "jle3DGraph.h"
#include "jle3DRenderer.h"
#include "jle3DSettings.h"
#include "jleEngineSettings.h"
#include "jleExplicitInclude.h"
#include "jleFramebufferMultisample.h"
#include "jleFramebufferScreen.h"
#include "jleFullscreenRendering.h"
#include "jleGame.h"
#include "jleInput.h"
#include "jleLuaEnvironment.h"
#include "jleMouseInput.h"
#include "jlePhysics.h"
#include "jleRenderThread.h"
#include "jleResourceRef.h"
#include "jleTimerManager.h"
#include "jleWindow.h"

#include <enet.h>
#include <plog/Log.h>
#include <soloud.h>

#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#include <RmlUi_Backend.h>
#include <shell/include/Shell.h>

#undef max
#undef min
#include <Tracy.hpp>

#include <3rdparty/WickedEngine/wiJobSystem.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

struct jleGameEngine::jleEngineInternal {
    jleResourceRef<jleEngineSettings> engineSettings;
};

jleGameEngine::jleGameEngine()
{
    LOGI << "Project built on: " << __DATE__ ", at " << __TIME__;

    LOGI << "Initializing job system, available hardware threads: " << std::thread::hardware_concurrency();
    wi::jobsystem::Initialize();

    gEngine = this;
    _resources = std::make_unique<jleResources>();

    _renderThread = std::make_unique<jleRenderThread>();

    LOGI << "Game Resources located at: " << jlePath{"GR:/"}.getRealPath();
    LOGI << "Engine Resources located at: " << jlePath{"ER:/"}.getRealPath();

    JLE_EXEC_IF(JLE_BUILD_EDITOR) { LOGI << "Editor Resources located at: " << jlePath{"ED:/"}.getRealPath(); }

    _internal = std::make_unique<jleEngineInternal>();
    _internal->engineSettings = jleResourceRef<jleEngineSettings>("GR:/settings/enginesettings.es");

    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS)
    {
        _window = std::make_unique<jleWindow>();

        PLOG_INFO << "Initializing the window";
        _window->settings(settings().windowSettings);
        _window->initWindow();

        _input = std::make_unique<jleInput>(_window);

        _3dRenderer = std::make_unique<jle3DRenderer>();
        _3dRenderGraph = std::make_unique<jle3DGraph>();
        _3dRendererSettings = std::make_unique<jle3DSettings>();
        _soLoud = std::make_unique<SoLoud::Soloud>();

        PLOG_INFO << "Initializing sound engine...";
        _soLoud->init();
    }

    LOG_INFO << "Starting the lua environment";
    _luaEnvironment = std::make_unique<jleLuaEnvironment>();
    _luaEnvironment->setupScriptLoader();

    _timerManager = std::make_unique<jleTimerManager>();

    if (settings().enableNetworking) {
        enet_initialize();
    }
}

jleGameEngine::~jleGameEngine()
{
    gEngine = nullptr;

    LOGI << "Shutting down job system";
    wi::jobsystem::ShutDown();

    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS)
    {
        PLOG_INFO << "Destroying the sound engine...";
        _soLoud->deinit();
    }

    if (settings().enableNetworking) {
        enet_deinitialize();
    }
}

void
jleGameEngine::startGame()
{
    if (!_gameCreator) {
        LOG_WARNING << "Game has not been set! Use SetGame<jleGameDerived>() "
                       "before starting the game.";
        return;
    }

    /*  if (!_physics) {
          // Re-initialize the physics
          _physics = std::make_unique<jlePhysics>();
      }*/

    game = _gameCreator();
    game->start();

    for (auto &scenePath : settings().initialScenesToLoad) {
        game->loadScene(scenePath);
    }
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
    LOG_VERBOSE << "Next frame dt: " << gEngine->deltaFrameTime();
    auto gameHaltedTemp = gameHalted;
    gameHalted = false;

    wi::jobsystem::context ctx;

    // Game thread
    wi::jobsystem::Execute(ctx, [&](wi::jobsystem::JobArgs args) { update(deltaFrameTime()); });

    // Render thread
    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS) { render(ctx); }
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
    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS)
    {
        constexpr int initialScreenX = 1024;
        constexpr int initialScreenY = 1024;
        mainScreenFramebuffer = std::make_shared<jleFramebufferScreen>(initialScreenX, initialScreenY);

        const auto &mouse = gEngine->input().mouse;
        mouse->setScreenSize(initialScreenX, initialScreenY);

        luaEnvironment()->loadScript("ER:/scripts/engine.lua");

        startRmlUi();

        _fullscreen_renderer = std::make_unique<jleFullscreenRendering>();

        JLE_EXEC_IF_NOT(JLE_BUILD_EDITOR)
        {
            const auto gameWindowResizeFunc = [&](const jleWindowResizeEvent& resizeEvent){
                gameWindowResizedEvent(resizeEvent.framebufferWidth, resizeEvent.framebufferHeight);
            };

            window().addWindowResizeCallback(gameWindowResizeFunc);
        }
    }
    LOG_INFO << "Starting the game engine";

    startGame();
}

void
jleGameEngine::resizeMainFramebuffer(unsigned int width, unsigned int height)
{
    renderThread().runOnRenderThread([this, width, height]() { mainScreenFramebuffer->resize(width, height); });

    const auto &inputMouse = gEngine->input().mouse;
    inputMouse->setScreenSize(width, height);
}

int
jleGameEngine::addGameWindowResizeCallback(std::function<void(unsigned int, unsigned int)> callback)
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
jleGameEngine::removeGameWindowResizeCallback(unsigned int callbackId)
{
    _gameWindowResizedCallbacks.erase(callbackId);
}

void
jleGameEngine::executeGameWindowResizedCallbacks(unsigned int w, unsigned int h)
{
    for (const auto &callback : _gameWindowResizedCallbacks) {
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
            JLE_SCOPE_PROFILE_CPU(jleGameEngine_parallelUpdates);
            game->parallelUpdates(dt);
        }

        {
            JLE_SCOPE_PROFILE_CPU(jleGameEngine_updateActiveScenes)
            game->updateActiveScenes(dt);
        }
        {
            JLE_SCOPE_PROFILE_CPU(RmlUi)
            // context->Update();
        }

        // physics().step(dt);
    }
}

void
jleGameEngine::render(wi::jobsystem::context& ctx)
{
    JLE_SCOPE_PROFILE_CPU(jleGameEngine_render)

    _renderThread->processRenderQueue();

    if (!gameHalted && game) {

        // Render to game view
        static jleFramebufferMultisample msaa{mainScreenFramebuffer->width(), mainScreenFramebuffer->height(), 4};

        if (mainScreenFramebuffer->width() != msaa.width() || mainScreenFramebuffer->height() != msaa.height()) {
            msaa.resize(mainScreenFramebuffer->width(), mainScreenFramebuffer->height());
        }

        if (_3dRenderGraphForRendering) {
            renderer().render(msaa, gameRef().mainCamera, *_3dRenderGraphForRendering, renderSettings());
        }

        // Render to the MSAA framebuffer, then blit the result over to the main framebuffer
        msaa.blitToOther(*mainScreenFramebuffer);

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

void
jleGameEngine::run()
{
    PLOG_INFO << "Starting the game loop";

    running = true;
    start();
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainLoopEmscripten, 0, true);
#else
    loop();
#endif
}
void
jleGameEngine::mainLoop()
{
    jleProfiler::NewFrame();
    JLE_SCOPE_PROFILE_CPU(mainLoop)

    auto frameStart = std::chrono::steady_clock::now();

    refreshDeltaTimes();

    _timerManager->process();

    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS) { input().mouse->updateDeltas(); }

    wi::jobsystem::context ctx;

    // Game thread
    wi::jobsystem::Execute(ctx, [&](wi::jobsystem::JobArgs args) { update(deltaFrameTime()); });
    // update(deltaFrameTime());

    // Render thread
    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS)
    {
        render(ctx);
        _window->updateWindow();
        running = !_window->windowShouldClose();
    }
    else
    {
        constexpr float simulationFrequency = 60.f;
        constexpr int sleepForMs = static_cast<int>(1000 / simulationFrequency);
        std::this_thread::sleep_until(frameStart + std::chrono::milliseconds(sleepForMs));
    }

    Wait(ctx);

    // Double buffer move
    _3dRenderGraphForRendering = std::move(_3dRenderGraph);
    _3dRenderGraph = std::make_unique<jle3DGraph>();

    FrameMark;
}
void
jleGameEngine::loop()
{
    while (running) {
        mainLoop();
    }
    exiting();
}
void
jleGameEngine::refreshDeltaTimes()
{
    ZoneScoped;
    JLE_EXEC_IF(JLE_BUILD_HEADLESS)
    {
        auto now = std::chrono::system_clock::now();
        auto milliseconds_since_epoch =
            std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        _currentFrame = static_cast<double>(milliseconds_since_epoch) * 0.001f;
    }
    else
    {
        _currentFrame = _window->time();
    }

    _deltaTime = _currentFrame - _lastFrame;
    _lastFrame = _currentFrame;
    _fps = static_cast<int>(1.0 / _deltaTime);
}
jle3DRenderer &
jleGameEngine::renderer()
{
    return *_3dRenderer.get();
}

float
jleGameEngine::lastFrameTime() const
{
    return _lastFrame;
}
float
jleGameEngine::currentFrameTime() const
{
    return _currentFrame;
}
float
jleGameEngine::deltaFrameTime() const
{
    return _deltaTime;
}
int
jleGameEngine::fps() const
{
    return _fps;
}
jleEngineSettings &
jleGameEngine::settings()
{
    return *_internal->engineSettings.get();
}
jle3DSettings &
jleGameEngine::renderSettings()
{
    return *_3dRendererSettings.get();
}

jleRenderThread &
jleGameEngine::renderThread()
{
    return *_renderThread;
}

jle3DGraph &
jleGameEngine::renderGraph()
{
    return *_3dRenderGraph.get();
}
jleInput &
jleGameEngine::input()
{
    return *_input;
}
jleWindow &
jleGameEngine::window()
{
    return *_window;
}
jleResources &
jleGameEngine::resources()
{
    return *_resources;
}
SoLoud::Soloud &
jleGameEngine::soLoud()
{
    return *_soLoud;
}
jleTimerManager &
jleGameEngine::timerManager()
{
    return *_timerManager;
}
