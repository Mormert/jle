// Copyright (c) 2023. Johan Lind

#pragma once

#include <memory>
#include <unordered_map>
#include <functional>

namespace SoLoud
{
class Soloud;
};
namespace Rml
{
class Context;
};
class Remotery;

class jleGame;
class jleResources;
class jleEngineSettings;
class jleInput;
class jleWindow;
class jleTimerManager;
class jlePhysics;
class jle3DRenderer;
class jle3DRendererSettings;
class jle3DRendererGraph;
class jleFullscreenRendering;
class jleFramebufferInterface;
class jleLuaEnvironment;

class jleGameEngine;
inline jleGameEngine *gEngine;

class jleGameEngine
{
public:
    virtual ~jleGameEngine();

    explicit jleGameEngine();

    template <class T>
    void
    setGame()
    {
        _gameCreator = []() { return std::make_unique<T>(); };
    }

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

    std::shared_ptr<jleFramebufferInterface> mainScreenFramebuffer;

    void resizeMainFramebuffer(unsigned int width, unsigned int height);

    int addGameWindowResizeCallback(std::function<void(unsigned int, unsigned int)> callback);

    void removeGameWindowResizeCallback(unsigned int callbackId);

    void executeGameWindowResizedCallbacks(unsigned int w, unsigned int h);

    void startGame();

    void restartGame();

    void killGame();

    void haltGame();

    void unhaltGame();

    void executeNextFrame();

    jlePhysics &physics();

    [[nodiscard]] bool isGameKilled() const;

    [[nodiscard]] bool isGameHalted() const;

    jleGame &gameRef();

    std::shared_ptr<jleLuaEnvironment> &luaEnvironment();

    static inline Rml::Context *context{};

private:
    void mainLoop();

    bool running{false};

    static void
    mainLoopEmscripten()
    {
        gEngine->mainLoop();
    }

    void loop();

    std::function<std::unique_ptr<jleGame>()> _gameCreator;

    std::unique_ptr<jleFullscreenRendering> _fullscreen_renderer;

    friend class jleGameEditorWindow;
    void gameWindowResizedEvent(unsigned int w, unsigned int h);

    std::unordered_map<unsigned int, std::function<void(unsigned int, unsigned int)>> _gameWindowResizedCallbacks;

protected:
    virtual void start();

    void startRmlUi();

    void killRmlUi();

    virtual void update(float dt);

    virtual void render();

    virtual void exiting();

    std::unique_ptr<jleResources> _resources;
    // std::unique_ptr<jleFontData> _fontData;
    std::unique_ptr<jleTimerManager> _timerManager;
    std::shared_ptr<jleWindow> _window;
    std::unique_ptr<jleInput> _input;
    std::unique_ptr<SoLoud::Soloud> _soLoud;

    friend class jleSceneEditorWindow;
    std::unique_ptr<jle3DRenderer> _3dRenderer;
    std::unique_ptr<jle3DRendererGraph> _3dRenderGraph;
    std::unique_ptr<jle3DRendererSettings> _3dRendererSettings;

    struct jleEngineInternal;
    std::unique_ptr<jleEngineInternal> _internal;

    void resetRenderGraphForNewFrame();

    jle3DRenderer &renderer();

    Remotery *_remotery;

    void refreshDeltaTimes();

    int _fps = 0;
    float _deltaTime = 0;
    float _currentFrame = 0;
    float _lastFrame = 0;

    std::unique_ptr<jlePhysics> _physics;
    std::shared_ptr<jleLuaEnvironment> _luaEnvironment;

    std::unique_ptr<jleGame> game;
    bool gameHalted = false;
};
