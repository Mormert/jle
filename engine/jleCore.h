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
//class jleFontData;
class jleInput;
class jleWindow;
class jleTimerManager;
class jlePhysics;
class jle3DRenderer;
class jle3DRendererSettings;
class jle3DRendererGraph;

class jleCore;
inline jleCore *gCore;

class jleCore
{
public:
    explicit jleCore();

    virtual ~jleCore();

    void run();

    jleTimerManager &timerManager();

    SoLoud::Soloud &soLoud();

    jleResources &resources();

    jleWindow &window();

    jleInput &input();

    jle3DRendererGraph &renderGraph();

    jle3DRendererSettings &renderSettings();

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
    //std::unique_ptr<jleFontData> _fontData;
    std::unique_ptr<jleTimerManager> _timerManager;
    std::shared_ptr<jleWindow> _window;
    std::shared_ptr<jleInput> _input;
    std::unique_ptr<SoLoud::Soloud> _soLoud;

    friend class jleSceneEditorWindow;
    std::unique_ptr<jle3DRenderer> _3dRenderer;
    std::unique_ptr<jle3DRendererGraph> _3dRenderGraph;
    std::unique_ptr<jle3DRendererSettings> _3dRendererSettings;

    struct jleCoreInternal;
    std::unique_ptr<jleCoreInternal> _internal;

    void resetRenderGraphForNewFrame();

    jle3DRenderer &renderer();

    Remotery *_remotery;

    void refreshDeltaTimes();

    int _fps = 0;
    float _deltaTime = 0;
    float _currentFrame = 0;
    float _lastFrame = 0;
};

#endif // JLE_CORE_H