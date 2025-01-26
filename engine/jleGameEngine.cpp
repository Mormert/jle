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
#include "core/jleResourceRef.h"
#include "jlECS/jlECS.h"
#include "jleEngineSettings.h"
#include "jleExplicitInclude.h"
#include "game/jleGame.h"
#include "game/jleGameRuntime.h"
#include "modules/graphics/core/jleFramebufferMultisample.h"
#include "modules/graphics/core/jleFullscreenRendering.h"
#include "modules/graphics/jle3DSettings.h"
#include "modules/graphics/jleFramePacket.h"
#include "modules/graphics/jleGraphics.h"
#include "modules/graphics/jleRenderThread.h"
#include "modules/input/jleInput.h"
#include "modules/scripting/jleLuaEnvironment.h"
#include "modules/windowing/jleWindow.h"

#include <plog/Log.h>

#undef max
#undef min
#include <Tracy.hpp>

#include <WickedEngine/wiJobSystem.h>
#include <modules/networking/jleNetworkingModule.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

struct jleGameEngine::jleEngineInternal {
    jleResourceRef<jleEngineSettings> engineSettings;
};

jleGameEngine::jleGameEngine(const EngineConstructConfig& config) :
    _engineConstructConfig{config}
{
    LOGI << "Project built on: " << __DATE__ ", at " << __TIME__;

    LOGI << "Initializing job system, available hardware threads: " << std::thread::hardware_concurrency();
    wi::jobsystem::Initialize();

    std::vector<std::string> directories;
    directories.push_back(jleVirtualPath{"GR:/"}.getRealPath().str());
    directories.push_back(jleVirtualPath{"ER:/"}.getRealPath().str());
    JLE_EXEC_IF(JLE_BUILD_EDITOR) { directories.push_back(jleVirtualPath{"ED:/"}.getRealPath().str()); }
    jleFileWatcher initialPathIndexing(directories);
    initialPathIndexing.sweep();

    _resources = std::make_unique<jleResourceHolder>();

    _renderThread = std::make_unique<jleRenderThread>();

    LOGI << "Game Resources located at: " << jleVirtualPath{"GR:/"}.getRealPath().str();
    LOGI << "Engine Resources located at: " << jleVirtualPath{"ER:/"}.getRealPath().str();

    JLE_EXEC_IF(JLE_BUILD_EDITOR) { LOGI << "Editor Resources located at: " << jleVirtualPath{"ED:/"}.getRealPath().str(); }

    jleSerializationContext serializationContext{
        .resources = _resources.get(),
        .serializationInterfaces = {_renderThread.get()}};

    _internal = std::make_unique<jleEngineInternal>();
    _internal->engineSettings = jleResourceRef<jleEngineSettings>(jlePath{jleVirtualPath{"GR:/settings/enginesettings.es"}}, serializationContext);

    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS)
    {
        _window = config.windowCreator();

        PLOG_INFO << "Initializing the window";
        _window->settings(_internal->engineSettings.get()->windowSettings);
        _window->initWindow();

        _input = std::make_unique<jleInput>(*_window);

        _3dRenderer = std::make_unique<jleGraphics>(serializationContext);
        _currentFramePacket = std::make_unique<jleFramePacket>();
        _3dRendererSettings = std::make_unique<jle3DSettings>();
    }

    jleNetworkingModule::initialize();

    _gameRuntime = std::make_unique<jleGameRuntime>(config.gameConfig, *this);
}

jleGameEngine::~jleGameEngine()
{
    LOGI << "Shutting down job system";
    wi::jobsystem::ShutDown();

    jleNetworkingModule::deinitialize();

    _gameRuntime.reset();
    _resources.reset();
}

void
jleGameEngine::start()
{
    auto serializationContext = createSerializationContext();
    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS)
    {
        _fullscreen_renderer = std::make_unique<jleFullscreenRendering>(serializationContext);

        JLE_EXEC_IF_NOT(JLE_BUILD_EDITOR)
        {
            const auto gameWindowResizeFunc = [&](const jleWindowResizeEvent &resizeEvent) {
                _gameRuntime->gameWindowResizedEvent(resizeEvent.framebufferWidth, resizeEvent.framebufferHeight);
            };

            _window->addWindowResizeCallback(gameWindowResizeFunc);
        }
    }
    LOG_INFO << "Starting the game engine";

    _gameRuntime->startGame(serializationContext);
}

void
jleGameEngine::render(jleCamera &camera, jleEngineUpdateContext &ctx, wi::jobsystem::context &jobsCtx)
{
    ZoneScoped;

    _renderThread->processRenderQueue();

    if (!ctx.gameRuntime.isGameHalted() && ctx.gameRuntime._game != nullptr) {

        // Render to game view
        static jleFramebufferMultisample msaa{
            ctx.gameRuntime.mainGameScreenFramebuffer->width(), ctx.gameRuntime.mainGameScreenFramebuffer->height(), 4};

        if (ctx.gameRuntime.mainGameScreenFramebuffer->width() != msaa.width() ||
            ctx.gameRuntime.mainGameScreenFramebuffer->height() != msaa.height()) {
            msaa.resize(ctx.gameRuntime.mainGameScreenFramebuffer->width(),
                        ctx.gameRuntime.mainGameScreenFramebuffer->height());
        }

        if (_previousFramePacket) {
            renderer().render(msaa, *_previousFramePacket);
        }

        // Render to the MSAA framebuffer, then blit the result over to the main framebuffer
        msaa.blitToOther(*ctx.gameRuntime.mainGameScreenFramebuffer);

        _fullscreen_renderer->renderFramebufferFullscreen(
            *ctx.gameRuntime.mainGameScreenFramebuffer, _window->width(), _window->height());
    }
}

void
jleGameEngine::exiting()
{
    _gameRuntime->killGame();
}

void
jleGameEngine::run()
{
    PLOG_INFO << "Starting the game loop";

    _running = true;
    start();
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
    ZoneScoped;

    auto frameStart = std::chrono::steady_clock::now();

    refreshDeltaTimes();

    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS) { _input->mouse.updateDeltas(); }

    wi::jobsystem::context jobsCtx;
    auto updateContext = createUpdateContext();

    // Copy the last frame's camera before splitting to game & render threads
    // TODO consider if we even need this ?
    // Especially now that the framepacket contains a camera... ??
    jleCamera camera = jleCamera{};
    if(!_gameRuntime->isGameKilled()){
        auto& ecs = _gameRuntime->getGame().getECS();
        for(auto [cameraComponent, transformComponent] : ecs.iterateMulti<cCamera, cTransform>()){
            cCamera::UpdateContext cameraUpdateCtx{
                .in ={ .transform = *transformComponent,
                       .width = _gameRuntime->mainGameScreenFramebuffer->width(),
                       .height = _gameRuntime->mainGameScreenFramebuffer->height()},
                .out = {.camera = camera}
            };
            cameraComponent->update(cameraUpdateCtx);
            break;
        }
    }

    // Game thread
    wi::jobsystem::Execute(jobsCtx, [&](wi::jobsystem::JobArgs args) { _gameRuntime->update(updateContext); });

    // Render thread
    JLE_EXEC_IF_NOT(JLE_BUILD_HEADLESS)
    {
        render(camera, updateContext, jobsCtx);
        _window->updateWindow();
        _running = !_window->windowShouldClose();
    }
    else
    {
        constexpr float simulationFrequency = 60.f;
        constexpr int sleepForMs = static_cast<int>(1000 / simulationFrequency);
        std::this_thread::sleep_until(frameStart + std::chrono::milliseconds(sleepForMs));
    }

    Wait(jobsCtx);

    // Double buffer COPY (todo: don't make a copy here, instead move it..)
    auto cpy = *_currentFramePacket;
    _previousFramePacket = std::make_unique<jleFramePacket>(cpy);
    _currentFramePacket->emptyQueues();

    FrameMark;
}
void
jleGameEngine::loop()
{
    while (_running) {
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

jleEngineSettings & jleGameEngine::getSettings() const {
    return *_internal->engineSettings.get();
}

jleGraphics &
jleGameEngine::renderer()
{
    return *_3dRenderer.get();
}

jleSerializationContext
jleGameEngine::createSerializationContext()
{
    std::vector<jleSerializableInterface *> interfaces;
    interfaces.push_back(_renderThread.get());

    if (auto *gameModules = getCurrentGameModules()) {
        _engineConstructConfig.gameConfig.populateSerializationInterfaces(*gameModules, interfaces);
    }

    return jleSerializationContext {
        .resources = _resources.get(),
        .serializationInterfaces = interfaces
    };
}

jleEngineUpdateContext
jleGameEngine::createUpdateContext()
{
    jleSerializationContext serializationContext = createSerializationContext();

    return jleEngineUpdateContext(*_gameRuntime,
                                  *_3dRenderer,
                                  *_renderThread,
                                  *_currentFramePacket,
                                  *_internal->engineSettings.get(),
                                  *_input,
                                  *_window,
                                  *_resources,
                                  _frameInfo,
                                  serializationContext);
}

jleGameModules*
jleGameEngine::getCurrentGameModules()
{
    if (!_gameRuntime->isGameKilled()) {
        return &_gameRuntime->getGame().getModules();
    }
    return nullptr;
}
