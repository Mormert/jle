// Copyright (c) 2023. Johan Lind

#ifndef JLE_CORE_H
#define JLE_CORE_H


#include "jleEngineSettings.h"
#include "jleTexture.h"
#include <memory>

namespace SoLoud
{
class Soloud;
};
class Remotery;

class jleFramebufferInterface;
class jleResources;
class jleFontData;
class jleInput;
class jleRendering;
class jleWindow;
class jleQuadRendering;
class jleTextRendering;
class jleTimerManager;

class jleCore;
inline jleCore *gCore;

class jleCore
{
public:
    jleCore(const jleCore &) = delete;
    jleCore(jleCore &&) = delete;
    jleCore &operator=(const jleCore &) = delete;
    jleCore &operator=(jleCore &&) = delete;

    explicit jleCore();

    virtual ~jleCore();

    void run();

    jleTimerManager &timerManager();

    SoLoud::Soloud &soLoud();

    jleResources &resources();

    jleWindow &window();

    jleInput &input();

    jleRendering &rendering();

    jleQuadRendering &quadRendering();

    jleTextRendering &textRendering();

    [[nodiscard]] jleEngineSettings &settings();

    [[nodiscard]] int fps() const;

    [[nodiscard]] float deltaFrameTime() const;

    [[nodiscard]] float currentFrameTime() const;

    [[nodiscard]] float lastFrameTime() const;

private:
    void loop();

    void mainLoop();

    bool running{false};

    static void
    mainLoopEmscripten()
    {
        gCore->mainLoop();
    }

    virtual void
    start()
    {
    }

    virtual void
    update(float dt)
    {
    }

    virtual void
    render()
    {
    }

    virtual void
    exiting()
    {
    }

protected:
    std::unique_ptr<jleResources> _resources;
    std::unique_ptr<jleFontData> _fontData;
    std::unique_ptr<jleTimerManager> _timerManager;
    const std::shared_ptr<jleWindow> _window;
    const std::shared_ptr<jleInput> _input;
    const std::shared_ptr<jleRendering> _rendering;
    const std::unique_ptr<SoLoud::Soloud> _soLoud;

    Remotery *_remotery;

    void refreshDeltaTimes();

    int _fps = 0;
    float _deltaTime = 0;
    float _currentFrame = 0;
    float _lastFrame = 0;
};

#endif // JLE_CORE_H