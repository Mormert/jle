// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleCoreSettings.h"
#include "jleInputAPI.h"
#include "jleNetworking.h"
#include "jleProfiler.h"
#include "jleRendering.h"
#include "jleTimerManager.h"

#include <memory>

class jleFramebuffer;
class jleResourceHolder;

struct CoreStatus_Internal {
public:
    int fps() { return _fps; }

    float deltaFrameTime() { return _deltaTime; }

    float currentFrameTime() { return _currentFrame; }

    float lastFrameTime() { return _lastFrame; }

    void refresh();

private:
    int _fps = 0;
    float _deltaTime = 0;
    float _currentFrame = 0;
    float _lastFrame = 0;
};

// Core part of the jle engine
class jleCore {
public:
    jleCore(const jleCore &) = delete;
    jleCore(jleCore &&) = delete;
    jleCore &operator=(const jleCore &) = delete;
    jleCore &operator=(jleCore &&) = delete;

    explicit jleCore(const std::shared_ptr<jleCoreSettings> &cs);

    virtual ~jleCore();

    void run();

    // Singleton
    static jleCore *core;

    // Entry point for a user to access the windowing API
    const std::shared_ptr<jleWindow> window;

    // Entry point for a user to access the input API
    const std::shared_ptr<jleInputAPI> input;

    // Entry point for a user to do fundamental rendering
    const std::shared_ptr<jleRendering> rendering;

    // Entry point for a user to get core status
    const std::shared_ptr<CoreStatus_Internal> status;

    jleTimerManager &timerManager();

    jleResourceHolder &resources() { return *_resources; }

private:
    void loop();

    void mainLoop();

    bool running{false};

    static void main_loop() { jleCore::core->mainLoop(); }

    // Internal impl data
    struct jleCoreInternalImpl;
    std::unique_ptr<jleCoreInternalImpl> coreImpl;

    friend struct CoreStatus_Internal;

protected:
    virtual void start() {}

    virtual void update(float dt) {}

    virtual void render() {}

    virtual void exiting() {}

    std::shared_ptr<jleCoreSettings> core_settings;
    std::shared_ptr<jleResourceHolder> _resources;

    jleTimerManager _timerManager;
};
