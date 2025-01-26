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
#include "game/jleGameRuntime.h"
#include "modules/jleEngineUpdateContext.h"
#include "plog/Appenders/ColorConsoleAppender.h"
#include "plog/Appenders/RollingFileAppender.h"
#include "plog/Formatters/TxtFormatter.h"

#include <functional>
#include <memory>

namespace wi::jobsystem
{
struct context;
}

class jleGame;
class jleResourceHolder;
class jleEngineSettings;
class jleWindow;
class jleGraphics;
class jle3DSettings;
class jleFramePacket;
class jleFullscreenRendering;
class jleFramebufferInterface;
class jleLuaEnvironment;
class jleRenderThread;
class jleGameRuntime;
class jleCamera;

class jleFrameInfo
{
public:
    [[nodiscard]] inline int getFps() const                 { return _fps;}
    [[nodiscard]] inline float getDeltaTime() const         { return _deltaTime; }
    [[nodiscard]] inline float getCurrentFrameTime() const  { return _currentFrame; }
    [[nodiscard]] inline float getLastFrameTime() const     { return _lastFrame; }

    jleFrameInfo() {
        _startTime = std::chrono::steady_clock::now();
        _lastFrameTimePoint = _startTime;
    }
private:
    int _fps = 0;
    float _deltaTime = 0;
    float _currentFrame = 0;
    float _lastFrame = 0;

    std::chrono::steady_clock::time_point _startTime;
    std::chrono::steady_clock::time_point _lastFrameTimePoint;

    friend class jleGameEngine;
};

class jleGameEngine
{
public:
    virtual ~jleGameEngine();

    struct EngineConstructConfig
    {
        std::function<std::unique_ptr<jleWindow>()> windowCreator = {};
        const jleGameConstructConfig& gameConfig;
    };

    explicit jleGameEngine(const EngineConstructConfig& config);

    void run();

    [[nodiscard]] const EngineConstructConfig& getEngineConstructConfig() const { return _engineConstructConfig; }
    [[nodiscard]] jleGameRuntime& getGameRuntime() const { return *_gameRuntime; }
    [[nodiscard]] jleResourceHolder& getResources() const { return *_resources; }
    [[nodiscard]] const jleFrameInfo& getFrameInfo() const { return _frameInfo; }
    [[nodiscard]] jleEngineSettings& getSettings() const;

    jleSerializationContext createSerializationContext();
    jleEngineUpdateContext createUpdateContext();

    void updateFrameInfo();
private:
    jleGameModules* getCurrentGameModules();

    void mainLoop();

    bool _running{false};

#ifdef __EMSCRIPTEN__
    static inline jleGameEngine *_emscriptenEnginePtr{};
    static void  mainLoopEmscripten() { _emscriptenEnginePtr->mainLoop(); }
#endif

    void loop();
    void start();

    std::unique_ptr<jleGameRuntime> _gameRuntime;
    std::unique_ptr<jleResourceHolder> _resources;

    struct jleEngineInternal;
    std::unique_ptr<jleEngineInternal> _internal;

    const EngineConstructConfig _engineConstructConfig;

    jleFrameInfo _frameInfo;

    plog::RollingFileAppender<plog::TxtFormatter> _loggingFileAppender;
    plog::ColorConsoleAppender<plog::TxtFormatter> _loggingConsoleAppender;
};
