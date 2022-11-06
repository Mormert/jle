// Copyright (c) 2022. Johan Lind

#include "jleCore.h"
#include "jleFont.h"
#include "jleKeyboardInput.h"
#include "jleMouseInput.h"
#include "jleRendering.h"
#include "jleResource.h"
#include "jleTexture.h"
#include "jleWindow.h"

#include <plog/Log.h>
#include <soloud.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <iostream>

jleCore::jleCore(const std::shared_ptr<jleCoreSettings> &cs)
    : _window{std::make_unique<jleWindow>()},
      _input{std::make_shared<jleInput>(
          std::make_shared<jleKeyboardInput>(_window),
          std::make_shared<jleMouseInput>(_window))},
      _rendering{std::make_shared<jleRendering>()},
      _resources{std::make_unique<jleResources>()},
      _soLoud{std::make_unique<SoLoud::Soloud>()} {
    PLOG_INFO << "Starting the core";

    _window->settings(cs->windowSettings);

    PLOG_INFO << "Starting the sound engine";
    _soLoud->init();

    _settings = cs;
}

jleCore::~jleCore() {
    PLOG_INFO << "Destroying the sound engine";
    _soLoud->deinit();
}

void jleCore::run() {
    if (gCore != nullptr) {
        std::cerr << "Error: Multiple instances of jleCore\n";
        exit(1);
    }

    gCore = this;

    PLOG_INFO << "Initializing the window";
    _window->initWindow(_rendering);

    PLOG_INFO << "Setting up rendering internals";
    _rendering->setup();
    _fontData = std::make_unique<jleFontData>();

    PLOG_INFO << "Starting the game loop";

    running = true;
    start();
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainLoopEmscripten, 0, true);
#else
    loop();
#endif
}

void jleCore::loop() {
    while (running) {
        mainLoop();
    }
    exiting();
}

void jleCore::mainLoop()
{
    jleProfiler::NewFrame();
    JLE_SCOPE_PROFILE(mainLoop)

    refreshDeltaTimes();

    _timerManager.process();

    input().mouse->updateDeltas();

    update(deltaFrameTime());

    render();
    _window->updateWindow();

    running = !_window->windowShouldClose();
}

jleTimerManager &jleCore::timerManager() { return _timerManager; }

void jleCore::refreshDeltaTimes() {
    _currentFrame = _window->time();
    _deltaTime = _currentFrame - _lastFrame;
    _lastFrame = _currentFrame;
    _fps = static_cast<int>(1.0 / _deltaTime);
}
