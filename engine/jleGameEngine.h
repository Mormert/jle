/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#pragma once

#include "jleCommon.h"

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

class jleGame;
class jleResources;
class jleEngineSettings;
class jleInput;
class jleWindow;
class jleTimerManager;
class jle3DRenderer;
class jle3DSettings;
class jle3DGraph;
class jleFullscreenRendering;
class jleFramebufferInterface;
class jleLuaEnvironment;
class jleRenderThread;

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

    jle3DGraph &renderGraph();

    jle3DSettings &renderSettings();

    jleRenderThread &renderThread();

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
    std::unique_ptr<jle3DGraph> _3dRenderGraph;
    std::unique_ptr<jle3DGraph> _3dRenderGraphForRendering;
    std::unique_ptr<jle3DSettings> _3dRendererSettings;
    std::unique_ptr<jleRenderThread> _renderThread;

    struct jleEngineInternal;
    std::unique_ptr<jleEngineInternal> _internal;

    jle3DRenderer &renderer();

    void refreshDeltaTimes();

    int _fps = 0;
    float _deltaTime = 0;
    double _currentFrame = 0;
    double _lastFrame = 0;

    std::shared_ptr<jleLuaEnvironment> _luaEnvironment;

    std::unique_ptr<jleGame> game;
    bool gameHalted = false;
};
