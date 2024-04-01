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

#include "jleLuaClass.h"
#include "jlePath.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol2/sol.hpp>

class jleLuaScript;
class jleFileIndexer;

class jleLuaEnvironment
{
public:
    jleLuaEnvironment();

    ~jleLuaEnvironment();

    void loadScript(const jlePath& path);

    void executeScript(const char* script);

    [[nodiscard]] sol::state& getState();

    void setupScriptLoader();

    std::unordered_map<jlePath, std::shared_ptr<jleLuaScript>>& loadedScripts();

    std::unordered_map<std::string, jleLuaClass>& loadedLuaClasses();

#if JLE_BUILD_EDITOR
    void loadNewlyAddedScripts();
#endif

private:
    void setupLua(sol::state& lua);

    void setupLuaGLM(sol::state& lua);

    std::unique_ptr<jleFileIndexer> _scriptFilesWatcher;

    std::unordered_map<std::string, jleLuaClass> _loadedLuaClasses;

    std::unordered_map<jlePath, std::shared_ptr<jleLuaScript>> _loadedScripts;

    sol::state _luaState;
};
