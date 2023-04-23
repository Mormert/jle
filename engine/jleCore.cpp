// Copyright (c) 2023. Johan Lind

#include "jleCore.h"
#include "jleExplicitInclude.h"
#include "jleFont.h"
#include "jleInput.h"
#include "jleKeyboardInput.h"
#include "jleMouseInput.h"
#include "jleProfiler.h"
#include "jleRendering.h"
#include "jleResource.h"
#include "jleTimerManager.h"
#include "jleWindow.h"

#include <plog/Log.h>
#include <soloud.h>
#include "Remotery/Remotery.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <iostream>

jleResourceRef<jleEngineSettings> g_CoreSettingsRef;

jleCore::jleCore()
    : _window{std::make_unique<jleWindow>()}, _input{std::make_shared<jleInput>(
                                                  std::make_shared<jleKeyboardInput>(_window),
                                                  std::make_shared<jleMouseInput>(_window))},
      _timerManager{std::make_unique<jleTimerManager>()},
      _rendering{std::make_shared<jleRendering>()},
      _resources{std::make_unique<jleResources>()}, _soLoud{std::make_unique<SoLoud::Soloud>()}
{
    PLOG_INFO << "Starting the core...";

    PLOG_INFO << "Initializing remote profiling...";
    rmtError error = rmt_CreateGlobalInstance(&_remotery);
    if( RMT_ERROR_NONE != error) {
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
    if (gCore != nullptr) {
        std::cerr << "Error: Multiple instances of jleCore\n";
        exit(1);
    }

    gCore = this;

    g_CoreSettingsRef.path = jlePath{"GR:settings/enginesettings.json"};
    g_CoreSettingsRef.loadResource();

    PLOG_INFO << "Initializing the window";
    _window->settings(settings().windowSettings);
    _window->initWindow(_rendering);

    PLOG_INFO << "Setting up rendering internals";
    _rendering->setup();
    _fontData = std::make_unique<jleFontData>();

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

jleRendering &
jleCore::rendering()
{
    return *_rendering;
}

jleQuadRendering &
jleCore::quadRendering()
{
    return _rendering->quads();
}

jleTextRendering &
jleCore::textRendering()
{
    return _rendering->texts();
}

jleEngineSettings &
jleCore::settings()
{
    return *g_CoreSettingsRef.get();
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

std::shared_ptr<jleEngineSettings>
jleCore::settingsPtr()
{
    return g_CoreSettingsRef.get();
}
