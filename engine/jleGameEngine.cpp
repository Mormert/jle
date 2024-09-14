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
#include "jleEngineSettings.h"
#include "jleExplicitInclude.h"
#include "jleGame.h"
#include "jleLuaEnvironment.h"
#include "jlePhysics.h"
#include "jleRenderThread.h"
#include "jleResourceRef.h"
#include "jleTimerManager.h"
#include "modules/game/jleGameRuntime.h"
#include "modules/graphics/core/jleFramebufferMultisample.h"
#include "modules/graphics/core/jleFramebufferScreen.h"
#include "modules/graphics/core/jleFullscreenRendering.h"
#include "modules/graphics/jle3DRenderer.h"
#include "modules/graphics/jle3DSettings.h"
#include "modules/graphics/jleFramePacket.h"
#include "modules/input/hardware/jleMouseInput.h"
#include "modules/input/jleInput.h"
#include "modules/windowing/jleWindow.h"

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

    _resources = std::make_unique<jleResources>();

    _renderThread = std::make_unique<jleRenderThread>();

    LOGI << "Game Resources located at: " << jlePath{"GR:/"}.getRealPath();
    LOGI << "Engine Resources located at: " << jlePath{"ER:/"}.getRealPath();

    JLE_EXEC_IF(JLE_BUILD_EDITOR) { LOGI << "Editor Resources located at: " << jlePath{"ED:/"}.getRealPath(); }

    LOG_INFO << "Starting the lua environment";
    _luaEnvironment = std::make_unique<jleLuaEnvironment>();

    jleSerializationContext serializationContext{_resources.get(), _luaEnvironment.get(), _renderThread.get()};

    _luaEnvironment->loadInitialScripts(serializationContext);

    _internal = std::make_unique<jleEngineInternal>();
    _internal->engineSettings = jleResourceRef<jleEngineSettings>("GR:/settings/enginesettings.es", serializationContext);

    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS)
    {
        _window = std::make_unique<jleWindow>();

        PLOG_INFO << "Initializing the window";
        _window->settings(settings().windowSettings);
        _window->initWindow();

        _input = std::make_unique<jleInput>(*_window);

        _3dRenderer = std::make_unique<jle3DRenderer>(serializationContext);
        _currentFramePacket = std::make_unique<jleFramePacket>();
        _3dRendererSettings = std::make_unique<jle3DSettings>();
        _soLoud = std::make_unique<SoLoud::Soloud>();

        PLOG_INFO << "Initializing sound engine...";
        _soLoud->init();
    }



    _timerManager = std::make_unique<jleTimerManager>();

    enet_initialize();

    _gameRuntime = std::make_unique<jleGameRuntime>(*this);

    _modulesContext = std::make_unique<jleEngineModulesContext>(*_gameRuntime,
                                                                *_3dRenderer,
                                                                *_renderThread,
                                                                *_3dRendererSettings,
                                                                *_currentFramePacket,
                                                                *_internal->engineSettings.get(),
                                                                *_input,
                                                                *_luaEnvironment,
                                                                *_window,
                                                                *_resources,
                                                                _frameInfo);
}

jleGameEngine::~jleGameEngine()
{
    LOGI << "Shutting down job system";
    wi::jobsystem::ShutDown();

    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS)
    {
        PLOG_INFO << "Destroying the sound engine...";
        _soLoud->deinit();
    }

    enet_deinitialize();

    _gameRuntime.reset();
    _resources.reset();
}

void
jleGameEngine::startRmlUi()
{

    auto width = 1024;
    auto height = 1024;

    if (!Shell::Initialize()) {
        LOGE << "Failed to init Shell for RmlUi";
        return;
    }

    // Constructs the system and render interfaces, creates a window, and attaches the renderer.
    if (!Backend::Initialize("RmlUiWindow", width, height, true, _window->glfwWindow())) {
        LOGE << "Failed to init backend for RmlUi";
        return;
    }

    // Install the custom interfaces constructed by the backend before initializing RmlUi.
    Rml::SetSystemInterface(Backend::GetSystemInterface());
    Rml::SetRenderInterface(Backend::GetRenderInterface());

    // RmlUi initialisation.
    Rml::Initialise();

    Rml::Log::Message(Rml::Log::LT_WARNING, "Test warning.");

    rmlContext_notUsed = Rml::CreateContext("main", Rml::Vector2i(width, height));
    if (!rmlContext_notUsed) {
        Rml::Shutdown();
        Backend::Shutdown();
        Shell::Shutdown();
        LOGE << "Failed to init backend for RmlUi";
        return;
    }

    Rml::Debugger::Initialise(rmlContext_notUsed);

    Shell::LoadFonts();

    Rml::LoadFontFace("C:/dev/cgfx/cgfx/GameResources/LatoLatin-Regular.ttf");
    Rml::LoadFontFace("C:/dev/cgfx/cgfx/GameResources/LatoLatin-Bold.ttf");
    Rml::LoadFontFace("C:/dev/cgfx/cgfx/GameResources/LatoLatin-Italic.ttf");
    Rml::LoadFontFace("C:/dev/cgfx/cgfx/GameResources/LatoLatin-BoldItalic.ttf");

    if (auto doc = rmlContext_notUsed->LoadDocument("assets/demo.rml")) {
        doc->Show();
    }

    Rml::Debugger::SetVisible(true);
}

void
jleGameEngine::start(jleEngineModulesContext &ctx)
{
    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS)
    {
        jleSerializationContext serializationContext{&ctx.resourcesModule, &ctx.luaEnvironment, &ctx.renderThread};

        luaEnvironment()->loadScript("ER:/scripts/engine.lua", serializationContext);
        luaEnvironment()->loadScript("ER:/scripts/globals.lua", serializationContext);

        startRmlUi();

        _fullscreen_renderer = std::make_unique<jleFullscreenRendering>(serializationContext);

        JLE_EXEC_IF_NOT(JLE_BUILD_EDITOR)
        {
            const auto gameWindowResizeFunc = [&](const jleWindowResizeEvent &resizeEvent) {
                ctx.gameRuntime.gameWindowResizedEvent(resizeEvent.framebufferWidth, resizeEvent.framebufferHeight);
            };

            window().addWindowResizeCallback(gameWindowResizeFunc);
        }
    }
    LOG_INFO << "Starting the game engine";

    _gameRuntime->startGame(ctx);
}

void
jleGameEngine::update(jleEngineModulesContext &ctx)
{
    ctx.gameRuntime.update(ctx);
}

void
jleGameEngine::render(jleEngineModulesContext &ctx, wi::jobsystem::context &jobsCtx)
{
    JLE_SCOPE_PROFILE_CPU(jleGameEngine_render)

    _renderThread->processRenderQueue();

    if (!ctx.gameRuntime.isGameHalted() && ctx.gameRuntime._game != nullptr) {

        // Render to game view
        static jleFramebufferMultisample msaa{
            ctx.gameRuntime.mainGameScreenFramebuffer->width(), ctx.gameRuntime.mainGameScreenFramebuffer->height(),
                                              4};

        if (ctx.gameRuntime.mainGameScreenFramebuffer->width() != msaa.width() ||
            ctx.gameRuntime.mainGameScreenFramebuffer->height() != msaa.height()) {
            msaa.resize(ctx.gameRuntime.mainGameScreenFramebuffer->width(),
                        ctx.gameRuntime.mainGameScreenFramebuffer->height());
        }

        if (_previousFramePacket) {
            jleSerializationContext serializationContext{&ctx.resourcesModule, &ctx.luaEnvironment, &ctx.renderThread};
            renderer().render(msaa,
                              ctx.gameRuntime.getGame().mainCamera,
                              *_previousFramePacket,
                              renderSettings(),
                              serializationContext);
        }

        // Render to the MSAA framebuffer, then blit the result over to the main framebuffer
        msaa.blitToOther(*ctx.gameRuntime.mainGameScreenFramebuffer);

        _fullscreen_renderer->renderFramebufferFullscreen(
            *ctx.gameRuntime.mainGameScreenFramebuffer, window().width(), window().height());
    }
}

void
jleGameEngine::exiting()
{
    _gameRuntime->killGame();
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
    start(*_modulesContext);
#ifdef __EMSCRIPTEN__
    _emscriptenEnginePtr = this;
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

    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS) { input().mouse.updateDeltas(); }

    wi::jobsystem::context jobsCtx;
    jleEngineModulesContext &modulesContext = *_modulesContext;

    // Game thread
    wi::jobsystem::Execute(jobsCtx, [&](wi::jobsystem::JobArgs args) { _gameRuntime->update(modulesContext); });
    // update(deltaFrameTime());

    // Render thread
    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS)
    {
        render(modulesContext, jobsCtx);
        _window->updateWindow();
        running = !_window->windowShouldClose();
    }
    else
    {
        constexpr float simulationFrequency = 60.f;
        constexpr int sleepForMs = static_cast<int>(1000 / simulationFrequency);
        std::this_thread::sleep_until(frameStart + std::chrono::milliseconds(sleepForMs));
    }

    Wait(jobsCtx);

    // Double buffer COPY (todo: don't make a copy here, instead move it..)
    _previousFramePacket = std::make_unique<jleFramePacket>(*_currentFramePacket);
    _currentFramePacket->emptyQueues();

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
        _frameInfo._currentFrame = static_cast<double>(milliseconds_since_epoch) * 0.001f;
    }
    else
    {
        _frameInfo._currentFrame = _window->time();
    }

    _frameInfo._deltaTime = _frameInfo._currentFrame - _frameInfo._lastFrame;
    _frameInfo._lastFrame = _frameInfo._currentFrame;
    _frameInfo._fps = static_cast<int>(1.0 / _frameInfo._deltaTime);
}
jle3DRenderer &
jleGameEngine::renderer()
{
    return *_3dRenderer.get();
}

float
jleGameEngine::lastFrameTime() const
{
    return _frameInfo._lastFrame;
}
float
jleGameEngine::currentFrameTime() const
{
    return _frameInfo._currentFrame;
}
float
jleGameEngine::deltaFrameTime() const
{
    return _frameInfo._deltaTime;
}
int
jleGameEngine::fps() const
{
    return _frameInfo._fps;
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

jleFramePacket &
jleGameEngine::currentFramePacket()
{
    return *_currentFramePacket.get();
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
