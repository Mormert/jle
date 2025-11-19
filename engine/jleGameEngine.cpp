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

#include "jleGameEngine.h"
#include "core/jleResourceRef.h"
#include "game/jleGame.h"
#include "game/jleGameRuntime.h"
#include "jlECS/jlECS.h"
#include "jleEngineSettings.h"
#include "jleExplicitInclude.h"
//#include "modules/graphics/jleGraphicsModule.h"
//#include "modules/graphics/jleRenderThread.h"
//#include "modules/scripting/jleLuaEnvironment.h"

#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include "core/jleDynamicLogAppender.h"

#include <Tracy.hpp>

#include <WickedEngine/wiJobSystem.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

struct jleGameEngine::jleEngineInternal {
    jleResourceRef<jleEngineSettings> engineSettings;
};

jleGameEngine::jleGameEngine(const EngineConstructConfig& config) : _engineConstructConfig{config},
    _loggingFileAppender("jle_log.plog", 1000000, 5), _loggingConsoleAppender{}
{
    plog::init<0>(plog::verbose, &_loggingFileAppender).addAppender(&_loggingConsoleAppender).addAppender(&dynamicAppender());

    LOGI << "Project built on: " << __DATE__ ", at " << __TIME__;

    LOGI << "Initializing job system, available hardware threads: " << std::thread::hardware_concurrency();
    wi::jobsystem::Initialize();

    std::vector<std::string> directories;
    directories.push_back(jleVirtualPath{"GR:/"}.getRealPath().str());
    directories.push_back(jleVirtualPath{"ER:/"}.getRealPath().str());
    JLE_EXEC_IF(JLE_BUILD_EDITOR) { directories.push_back(jleVirtualPath{"ED:/"}.getRealPath().str()); }
    jleFileWatcher initialPathIndexing(directories);
    initialPathIndexing.sweep();

    _resources = std::make_unique<jleResourceHolder>();

    LOGI << "Game Resources located at: " << jleVirtualPath{"GR:/"}.getRealPath().str();
    LOGI << "Engine Resources located at: " << jleVirtualPath{"ER:/"}.getRealPath().str();

    JLE_EXEC_IF(JLE_BUILD_EDITOR) { LOGI << "Editor Resources located at: " << jleVirtualPath{"ED:/"}.getRealPath().str(); }

    jleSerializationContext serializationContext{
        .resources = _resources.get(),
        .serializationInterfaces = {}};

    _internal = std::make_unique<jleEngineInternal>();
    _internal->engineSettings = jleResourceRef<jleEngineSettings>(jlePath{jleVirtualPath{"GR:/settings/enginesettings.es"}}, serializationContext);

    _gameRuntime = std::make_unique<jleGameRuntime>(config.gameConfig);
}

jleGameEngine::~jleGameEngine()
{
    LOGI << "Shutting down job system";
    wi::jobsystem::ShutDown();

    _gameRuntime.reset();
    _resources.reset();
}

void
jleGameEngine::start()
{
    auto serializationContext = createSerializationContext();
    _gameRuntime->startGame(serializationContext);
}

void
jleGameEngine::run()
{
    PLOG_INFO << "Starting the game loop";

    _running = true;
    start();
#ifndef __EMSCRIPTEN__
    loop();
#else
    _emscriptenEnginePtr = this;
    emscripten_set_main_loop(mainLoopEmscripten, 0, true);
#endif
}

void
jleGameEngine::mainLoop()
{
    ZoneScoped;

    updateFrameInfo();

    auto updateContext = createUpdateContext();

    _gameRuntime->processGameReset(updateContext);

    // Game thread and render thread are synced here
    if(auto preRender = _engineConstructConfig.gameConfig.modulesPreRender){
        preRender(*getCurrentGameModules(), updateContext.serializationContext);
    }

    // Game thread
    wi::jobsystem::context jobsCtx;
    wi::jobsystem::Execute(jobsCtx, [&](wi::jobsystem::JobArgs args) { _gameRuntime->update(updateContext); });

    // Render thread
    if (auto render = _engineConstructConfig.gameConfig.modulesRender){
        bool windowShouldClose = render(*getCurrentGameModules(), updateContext.serializationContext);
        _running = !windowShouldClose;
    }

    Wait(jobsCtx);

    // Game thread and render thread are synced here
    if (auto postRender = _engineConstructConfig.gameConfig.modulesPostRender) {
        postRender(*getCurrentGameModules(), updateContext.serializationContext);
    }

    FrameMark;
}

void
jleGameEngine::loop()
{
    while (_running) {
        mainLoop();
    }
}

void

jleGameEngine::updateFrameInfo()
{
    ZoneScoped;

    const auto now = std::chrono::steady_clock::now();

    _frameInfo._currentFrame = std::chrono::duration<float>(now - _frameInfo._startTime).count();
    _frameInfo._deltaTime = std::chrono::duration<float>(now - _frameInfo._lastFrameTimePoint).count();
    _frameInfo._fps = (_frameInfo._deltaTime > 0.0f) ? static_cast<int>(1.0f / _frameInfo._deltaTime) : 0;
    _frameInfo._lastFrame = std::chrono::duration<float>(_frameInfo._lastFrameTimePoint - _frameInfo._startTime).count();
    _frameInfo._lastFrameTimePoint = now;
}

jleEngineSettings & jleGameEngine::getSettings() const {
    return *_internal->engineSettings.get();
}

jleSerializationContext
jleGameEngine::createSerializationContext()
{
    std::vector<jleSerializableInterface *> interfaces;

    if (auto *gameModules = getCurrentGameModules()) {
        _engineConstructConfig.gameConfig.populateSerializationInterfaces(*gameModules, interfaces);
    }

    return jleSerializationContext {
        .resources = _resources.get(),
        .serializationInterfaces = interfaces,
        .modules = getCurrentGameModules() // TODO: In the future, don't use this
    };
}

jleEngineUpdateContext
jleGameEngine::createUpdateContext()
{
    jleSerializationContext serializationContext = createSerializationContext();

    return jleEngineUpdateContext(*_gameRuntime,
                                  *_internal->engineSettings.get(),
                                  *_resources,
                                  _frameInfo,
                                  serializationContext);
}

jleGameModules*
jleGameEngine::getCurrentGameModules()
{
    if (!_gameRuntime->isGameKilled()) {
        return &_gameRuntime->getGame().getModules();
    }
    return nullptr;
}
