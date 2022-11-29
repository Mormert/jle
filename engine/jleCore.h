// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleCoreSettings.h"
#include "jleInput.h"
#include "jleNetworking.h"
#include "jleProfiler.h"
#include "jleRendering.h"
#include "jleTimerManager.h"

#include <memory>

namespace SoLoud {
class Soloud;
};

class jleFramebufferInterface;
class jleResources;
class jleFontData;

class jleCore;
inline jleCore *gCore;

// Core class jle
class jleCore {
public:
    jleCore(const jleCore &) = delete;
    jleCore(jleCore &&) = delete;
    jleCore &operator=(const jleCore &) = delete;
    jleCore &operator=(jleCore &&) = delete;

    explicit jleCore(const std::shared_ptr<jleCoreSettings> &cs);

    virtual ~jleCore();

    void run();

    jleTimerManager &timerManager();

    inline SoLoud::Soloud &soLoud() { return *_soLoud; }

    inline jleResources &resources() { return *_resources; }

    inline jleWindow &window() { return *_window; }

    inline jleInput &input() { return *_input; }

    inline jleRendering &rendering() { return *_rendering; }

    inline jleQuadRendering &quadRendering() { return _rendering->quads(); }

    inline jleTextRendering &textRendering() { return _rendering->texts(); }

    [[nodiscard]] inline const jleCoreSettings &settings() const {
        return *_settings;
    }

    [[nodiscard]] inline int fps() const { return _fps; }

    [[nodiscard]] inline float deltaFrameTime() const { return _deltaTime; }

    [[nodiscard]] inline float currentFrameTime() const {
        return _currentFrame;
    }

    [[nodiscard]] inline float lastFrameTime() const { return _lastFrame; }

private:
    void loop();

    void mainLoop();

    bool running{false};

    static void mainLoopEmscripten() { gCore->mainLoop(); }

    virtual void start() {}

    virtual void update(float dt) {}

    virtual void render() {}

    virtual void exiting() {}

    std::shared_ptr<jleCoreSettings> _settings;
    std::unique_ptr<jleResources> _resources;
    std::unique_ptr<jleFontData> _fontData;
    const std::shared_ptr<jleWindow> _window;
    const std::shared_ptr<jleInput> _input;
    const std::shared_ptr<jleRendering> _rendering;
    const std::unique_ptr<SoLoud::Soloud> _soLoud;

    jleTimerManager _timerManager;

    void refreshDeltaTimes();

    int _fps = 0;
    float _deltaTime = 0;
    float _currentFrame = 0;
    float _lastFrame = 0;
};
