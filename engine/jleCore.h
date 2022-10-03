// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleCoreSettings.h"
#include "jleInputAPI.h"
#include "jleNetworking.h"
#include "jleProfiler.h"
#include "jleRendering.h"
#include "jleTimerManager.h"

#include <memory>

class Framebuffer_OpenGL;

struct CoreStatus_Internal {
public:
    int GetFPS() { return fps; }

    float GetDeltaFrameTime() { return deltaTime; }

    float GetCurrentFrameTime() { return currentFrame; }

    float GetLastFrameTime() { return lastFrame; }

    void Refresh();

private:
    int fps = 0;
    float deltaTime = 0;
    float currentFrame = 0;
    float lastFrame = 0;
};

// Core part of the jle engine
class jleCore {
public:
    jleCore(const jleCore&) = delete;
    jleCore(jleCore&&) = delete;
    jleCore& operator=(const jleCore&) = delete;
    jleCore& operator=(jleCore&&) = delete;

    explicit jleCore(const std::shared_ptr<jleCoreSettings>& cs);

    virtual ~jleCore();

    void Run();

    // Singleton
    static jleCore *core;

    // Entry point for a user to access the windowing API
    const std::shared_ptr<jleWindow_GLFW_OpenGL> window;

    // Entry point for a user to access the input API
    const std::shared_ptr<jleInputAPI> input;

    // Entry point for a user to do fundamental rendering
    const std::shared_ptr<jleRendering> rendering;

    // Entry point for a user to get core status
    const std::shared_ptr<CoreStatus_Internal> status;

    jleTimerManager& GetTimerManager();

private:
    void Loop();

    void MainLoop();

    bool running{false};

    static void main_loop() { jleCore::core->MainLoop(); }

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
