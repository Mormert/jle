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

#include "core/jleCommon.h"
#include "modules/game/jleGameRuntime.h"
#include "modules/jleEngineModulesContext.h"

#include <functional>
#include <memory>
#include <unordered_map>

namespace SoLoud
{
class Soloud;
};
namespace Rml
{
class Context;
};

namespace wi::jobsystem
{
struct context;
}

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
class jleGameRuntime;

class jleFrameInfo
{
public:
    // clang-format off
    [[nodiscard]] inline int getFps() const                 { return _fps;}
    [[nodiscard]] inline float getDeltaTime() const         { return _deltaTime; }
    [[nodiscard]] inline double getCurrentFrameTime() const { return _currentFrame; }
    [[nodiscard]] inline double getLastFrameTime() const    { return _lastFrame; }
    // clang-format on

private:
    int _fps = 0;
    float _deltaTime = 0;
    double _currentFrame = 0;
    double _lastFrame = 0;

    friend class jleGameEngine;
};

class jleGameEngine
{
public:
    virtual ~jleGameEngine();

    explicit jleGameEngine();

    template <class T>void setGame(){_gameRuntime->_gameCreator = []() { return std::make_unique<T>(); };}

    void run();

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

    std::shared_ptr<jleLuaEnvironment> &luaEnvironment();

    static inline Rml::Context *rmlContext_notUsed{};

private:
    std::unique_ptr<jleEngineModulesContext> _modulesContext;

    void mainLoop();

    bool running{false};

#ifdef __EMSCRIPTEN__
    static jleGameEngine *_emscriptenEnginePtr;
    static void
    mainLoopEmscripten()
    {
        _emscriptenEnginePtr->mainLoop();
    }
#endif

    void loop();

    std::unique_ptr<jleFullscreenRendering> _fullscreen_renderer;

protected:
    virtual void start(jleEngineModulesContext &context);

    void startRmlUi();

    void killRmlUi();

    virtual void update(jleEngineModulesContext &ctx);

    virtual void render(jleEngineModulesContext &ctx, wi::jobsystem::context &jobsCtx);

    virtual void exiting();

    friend class jleGameRuntime;
    std::unique_ptr<jleGameRuntime> _gameRuntime;

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
    jleFrameInfo _frameInfo{};

    std::shared_ptr<jleLuaEnvironment> _luaEnvironment;
};
