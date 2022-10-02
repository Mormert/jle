// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleCoreSettings.h"
#include "jleInputAPI.h"
#include "jleNetworking.h"
#include "jleProfiler.h"
#include "jleRendering.h"
#include "jleTimerManager.h"
#include "jleWindowAPIInterface.h"
#include "jleWindowFactoryInterface.h"
#include "no_copy_no_move.h"

#include <memory>

class Framebuffer_OpenGL;

struct jleCoreStatus {
    virtual ~jleCoreStatus() = default;

    virtual int GetFPS() = 0;

    virtual float GetDeltaFrameTime() = 0;

    virtual float GetCurrentFrameTime() = 0;

    virtual float GetLastFrameTime() = 0;
};

// Core part of the jle engine
class jleCore {
public:
    NO_COPY_NO_MOVE(jleCore)

    explicit jleCore(const std::shared_ptr<jleCoreSettings>& cs);

    virtual ~jleCore();

    void Run();

    const std::unique_ptr<jleRenderingFactoryInterface> renderingFactory;
    const std::unique_ptr<jleWindowFactoryInterface> windowFactory;

    // Singleton
    static jleCore *core;

    // Entry point for a user to access the windowing API
    const std::shared_ptr<jleWindowAPIInterface> window;

    // Entry point for a user to access the input API
    const std::shared_ptr<jleInputAPI> input;

    // Entry point for a user to do fundamental rendering
    const std::shared_ptr<jleRendering> rendering;

    // Entry point for a user to get core status
    const std::shared_ptr<jleCoreStatus> status;

    jleTimerManager& GetTimerManager();

private:
    void Loop();

    void MainLoop();

    bool running{false};

    static void main_loop() { jleCore::core->MainLoop(); }

    const std::unique_ptr<jleWindowInitializerInterface> window_initializer;

    // Internal impl data
    struct jleCoreInternalImpl;
    std::unique_ptr<jleCoreInternalImpl> coreImpl;

    friend struct CoreStatus_Internal;

protected:
    virtual void Start() {}

    virtual void Update(float dt) {}

    virtual void Render() {}

    virtual void Exiting() {}

    std::shared_ptr<jleCoreSettings> core_settings;

    jleTimerManager _timerManager;
};
