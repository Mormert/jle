// Copyright (c) 2022. Johan Lind

#include "jleCore.h"
#include "jleSoLoud.h"

#include "jleKeyboardInputInternal.h"
#include "jleMouseInputInternal.h"

#include "jleFont.h"
#include "jleRenderingFactory_OpenGL.h"
#include "jleStaticOpenGLState.h"
#include "jleTextureCreator_OpenGL.h"
#include "jleWindowFactory_GLFW.h"

#include <plog/Log.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <iostream>

jleCore *jleCore::core{nullptr};

struct CoreStatus_Internal;
struct jleCore::jleCoreInternalImpl {
    std::shared_ptr<jleRenderingAPI_OpenGL> rendering_internal;
    std::shared_ptr<jleWindowInternalAPIInterface> window_internal;
    std::shared_ptr<CoreStatus_Internal> status_internal;
};

struct CoreStatus_Internal : jleCoreStatus {
public:
    int GetFPS() override { return fps; }

    float GetDeltaFrameTime() override { return deltaTime; }

    float GetCurrentFrameTime() override { return currentFrame; }

    float GetLastFrameTime() override { return lastFrame; }

    void Refresh() {
        currentFrame = jleCore::core->coreImpl->window_internal->GetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        fps = static_cast<int>(1.0 / deltaTime);
    }

private:
    int fps = 0;
    float deltaTime = 0;
    float currentFrame = 0;
    float lastFrame = 0;
};

std::unique_ptr<jleRenderingFactoryInterface> CreateRenderingFactory(
    jleEngineInternalAPISettings& eia) {
    switch (eia.renderingAPI) {
    case jleEngineInternalAPISettings::RenderingAPI::OPENGL_33:
        return std::make_unique<jleRenderingFactory_OpenGL>();
    default:
        std::cerr << "Setting rendering API failed!\n";
        exit(1);
        break;
    }
}

std::unique_ptr<jleWindowFactoryInterface> CreateWindowFactory(
    jleEngineInternalAPISettings& eia) {
    switch (eia.windowingAPI) {
    case jleEngineInternalAPISettings::WindowAPI::GLFW:
        return std::make_unique<jleWindowFactory_GLFW>();
    default:
        std::cerr << "Setting window API failed!\n";
        exit(1);
        break;
    }
}

jleCore::jleCore(const std::shared_ptr<jleCoreSettings>& cs)
    : renderingFactory{CreateRenderingFactory(cs->engineAPIs)},
      windowFactory{CreateWindowFactory(cs->engineAPIs)},
      window{windowFactory->CreateWin()},
      rendering{renderingFactory->CreateRenderingAPI()},
      input{std::make_shared<jleInputAPI>(
          std::make_shared<jleKeyboardInputInternal>(
              std::static_pointer_cast<jleWindowInternalAPIInterface>(window)),
          std::make_shared<jleMouseInputInternal>(
              std::static_pointer_cast<jleWindowInternalAPIInterface>(
                  window)))},
      window_initializer{windowFactory->CreateWindowInitializer()},
      texture_creator{std::make_shared<jleTextureCreator_OpenGL>()},
      status{std::make_shared<CoreStatus_Internal>()} {
    PLOG_INFO << "Starting the core";
    coreImpl = std::make_unique<jleCoreInternalImpl>();
    coreImpl->rendering_internal =
        std::static_pointer_cast<jleRenderingAPI_OpenGL>(rendering);
    coreImpl->window_internal =
        std::static_pointer_cast<jleWindowInternalAPIInterface>(window);
    coreImpl->status_internal =
        std::static_pointer_cast<CoreStatus_Internal>(status);

    coreImpl->window_internal->SetWindowSettings(cs->windowSettings);

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
    coreImpl->window_internal->InitWindow(*window_initializer,
                                          coreImpl->rendering_internal);

    jleFont::Init();

    PLOG_INFO << "Setting up rendering internals";
    coreImpl->rendering_internal->Setup(*renderingFactory);

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

    coreImpl->status_internal->Refresh();

    _timerManager.Process();

    Update(status->GetDeltaFrameTime());

    Render();
    ((jleWindowInternalAPIInterface *)window.get())->UpdateWindow();

    running =
        !((jleWindowInternalAPIInterface *)window.get())->WindowShouldClose();
}

jleTimerManager& jleCore::GetTimerManager() { return _timerManager; }
