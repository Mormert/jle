// Copyright (c) 2022. Johan Lind

#include "jleCore.h"
#include "jleFont.h"
#include "jleKeyboardInput.h"
#include "jleMouseInput.h"
#include "jleRendering.h"
#include "jleSoLoud.h"
#include "jleStaticOpenGLState.h"
#include "jleTexture.h"
#include "jleWindow.h"

#include <plog/Log.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <iostream>

jleCore *jleCore::core{nullptr};

struct jleCore::jleCoreInternalImpl {
    std::shared_ptr<jleRendering> renderingInternal;
    std::shared_ptr<jleWindow> windowInternal;
    std::shared_ptr<CoreStatus_Internal> statusInternal;
};
jleCore::jleCore(const std::shared_ptr<jleCoreSettings>& cs)
    : window{std::make_unique<jleWindow>()},
      input{std::make_shared<jleInputAPI>(
          std::make_shared<jleKeyboardInput>(window),
          std::make_shared<jleMouseInput>(

              window))},
      rendering{std::make_shared<jleRendering>()},
      status{std::make_shared<CoreStatus_Internal>()} {
    PLOG_INFO << "Starting the core";
    coreImpl = std::make_unique<jleCoreInternalImpl>();
    coreImpl->renderingInternal = rendering;
    coreImpl->windowInternal = window;
    coreImpl->statusInternal = status;

    coreImpl->windowInternal->SetWindowSettings(cs->windowSettings);

    jleSoLoud::Init();

    core_settings = cs;
}

jleCore::~jleCore() {
    jleSoLoud::DeInit();
    jleFont::DeInit();
}

void jleCore::Run() {
    if (core != nullptr) {
        std::cerr << "Error: Multiple instances of jleCore\n";
        exit(1);
    }
    core = this;

    PLOG_INFO << "Initializing the window";
    coreImpl->windowInternal->InitWindow(coreImpl->renderingInternal);

    jleFont::Init();

    PLOG_INFO << "Setting up rendering internals";
    coreImpl->renderingInternal->Setup();

    PLOG_INFO << "Starting the game loop";

    running = true;
    Start();
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
#else
    Loop();
#endif
}

void jleCore::Loop() {
    while (running) {
        MainLoop();
    }
    Exiting();
}

void jleCore::MainLoop() {
    jleProfiler::NewFrame();
    JLE_SCOPE_PROFILE(MainLoop)

    coreImpl->statusInternal->Refresh();

    _timerManager.Process();

    Update(status->GetDeltaFrameTime());

    Render();
    window->UpdateWindow();

    running = !window->WindowShouldClose();
}

jleTimerManager& jleCore::GetTimerManager() { return _timerManager; }

void CoreStatus_Internal::Refresh() {
    currentFrame = jleCore::core->coreImpl->windowInternal->GetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    fps = static_cast<int>(1.0 / deltaTime);
}
