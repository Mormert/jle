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
#include "core/jleFileWatcher.h"
#include "core/jlePath.h"
#include <core/serialization/jleSerialization.h>

#include <future>

#include "jleLuaClass.h"

class jleLuaScript;
class jleFileWatcher;
struct jleSerializationContext;

namespace sol
{
class state;
}

class jleLuaEnvironment : public jleSerializableInterface
{
public:
    jleLuaEnvironment();

    ~jleLuaEnvironment() override;

    void loadScript(const jlePath &path, jleSerializationContext &ctx);

    void executeScript(const char *script);

    [[nodiscard]] sol::state &getState();

    void loadInitialScripts(jleSerializationContext &ctx);

    std::unordered_map<jlePath, std::shared_ptr<jleLuaScript>> &loadedScripts();

    std::unordered_map<std::string, jleLuaClass> &loadedLuaClasses();

    void loadNewlyAddedScripts(jleSerializationContext &ctx);

protected:
    virtual void setupLua(sol::state &lua);

    void setupLuaGLM(sol::state &lua);

    jleFileWatcher _scriptFilesWatcher;
    std::future<jleFileIndexerResult> _fileWatchFuture;

    std::unordered_map<std::string, jleLuaClass> _loadedLuaClasses;

    std::unordered_map<jlePath, std::shared_ptr<jleLuaScript>> _loadedScripts;

    std::unique_ptr<sol::state> _luaState;
};
