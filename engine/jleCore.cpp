// Copyright (c) 2023. Johan Lind

#include "jleCore.h"
#include "jle3DRenderer.h"
#include "jleExplicitInclude.h"
#include "jleFont.h"
#include "jleInput.h"
#include "jleKeyboardInput.h"
#include "jleMouseInput.h"
#include "jleProfiler.h"
#include "jleResource.h"
#include "jleTimerManager.h"
#include "jleWindow.h"

#include "Remotery/Remotery.h"
#include <plog/Log.h>
#include <soloud.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <iostream>

struct jleCore::jleCoreInternal{
    jleResourceRef<jleEngineSettings> engineSettings;
};

jleCore::jleCore()
{
    gCore = this;

    _resources = std::make_unique<jleResources>();

    _internal = std::make_unique<jleCoreInternal>();
    _internal->engineSettings = jleResourceRef<jleEngineSettings>("GR:/settings/enginesettings.es");

    _window = std::make_unique<jleWindow>();

    PLOG_INFO << "Initializing the window";
    _window->settings(settings().windowSettings);
    _window->initWindow();

    _input = std::make_shared<jleInput>(std::make_shared<jleKeyboardInput>(_window),
                                      std::make_shared<jleMouseInput>(_window));

    _timerManager = std::make_unique<jleTimerManager>();


    _3dRenderer = std::make_unique<jle3DRenderer>();
    _3dRenderGraph = std::make_unique<jle3DRendererGraph>();
    _3dRendererSettings = std::make_unique<jle3DRendererSettings>();
    _soLoud = std::make_unique<SoLoud::Soloud>();

    PLOG_INFO << "Starting the core...";

    PLOG_INFO << "Initializing remote profiling...";
    rmtError error = rmt_CreateGlobalInstance(&_remotery);
    if (RMT_ERROR_NONE != error) {
        PLOG_ERROR << "Error launching Remotery: " << error;
        std::exit(EXIT_FAILURE);
    }
    rmt_SetCurrentThreadName("Main Thread");

    PLOG_INFO << "Initializing sound engine...";
    _soLoud->init();
}

jleCore::~jleCore()
{
    PLOG_INFO << "Destroying the sound engine...";
    _soLoud->deinit();

    PLOG_INFO << "Destroying the remote profiling...";
    rmt_UnbindOpenGL();
    rmt_DestroyGlobalInstance(_remotery);
}

void
jleCore::run()
{
    //PLOG_INFO << "Setting up rendering internals";
    //_fontData = std::make_unique<jleFontData>();

    PLOG_INFO << "Binding Remotery to OpenGL";
    rmt_BindOpenGL();

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
jleCore::loop()
{
    while (running) {
        mainLoop();
    }
    exiting();
}

void
jleCore::mainLoop()
{
    jleProfiler::NewFrame();
    JLE_SCOPE_PROFILE_CPU(mainLoop)

    refreshDeltaTimes();

    _timerManager->process();

    input().mouse->updateDeltas();

    update(deltaFrameTime());

    render();
    _window->updateWindow();

    running = !_window->windowShouldClose();
}

jleTimerManager &
jleCore::timerManager()
{
    return *_timerManager;
}

void
jleCore::refreshDeltaTimes()
{
    _currentFrame = _window->time();
    _deltaTime = _currentFrame - _lastFrame;
    _lastFrame = _currentFrame;
    _fps = static_cast<int>(1.0 / _deltaTime);
}

SoLoud::Soloud &
jleCore::soLoud()
{
    return *_soLoud;
}

jleResources &
jleCore::resources()
{
    return *_resources;
}

jleWindow &
jleCore::window()
{
    return *_window;
}

jleInput &
jleCore::input()
{
    return *_input;
}

jleEngineSettings &
jleCore::settings()
{
    return *_internal->engineSettings.get();
}

int
jleCore::fps() const
{
    return _fps;
}

float
jleCore::deltaFrameTime() const
{
    return _deltaTime;
}
float
jleCore::currentFrameTime() const
{
    return _currentFrame;
}

float
jleCore::lastFrameTime() const
{
    return _lastFrame;
}

jle3DRendererGraph &
jleCore::renderGraph()
{
    return *_3dRenderGraph.get();
}

jle3DRenderer &
jleCore::renderer()
{
    return *_3dRenderer.get();
}

void
jleCore::resetRenderGraphForNewFrame()
{
    _3dRenderGraph = std::make_unique<jle3DRendererGraph>();
}

jle3DRendererSettings &
jleCore::renderSettings()
{
    return *_3dRendererSettings.get();
}
