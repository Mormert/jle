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

#include "jleLuaScript.h"
#include "jleGameEngine.h"
#include "jleLuaEnvironment.h"

bool
jleLuaScript::loadFromFile(const jlePath &path)
{
    std::ifstream load{path.getRealPath()};
    if (!load.good()) {
        return false;
    }

    std::stringstream buffer;
    buffer << load.rdbuf();

    _sourceCode = buffer.str();
    _luaScriptName = path.getFileNameNoEnding();

    return true;
}

void
jleLuaScript::loadScriptIntoLuaEnv(jleLuaEnvironment& luaEnvironment)
{
    try {
        const auto absoluteSrcCodePath = path.getRealPath();
        luaEnvironment.getState().script(_sourceCode, absoluteSrcCodePath);
        _failsLoading = false;

        const auto classes = jleLuaClass::getLuaClassesFromLuaSrc(path, _sourceCode);

        for (auto &c : classes) {
            const auto &className = c.getClassName();
            luaEnvironment.loadedLuaClasses()[className] = c;
        }

    } catch (std::exception &e) {
        LOGE << "Loading script failed: " << e.what();
        _failsLoading = true;
    }
    luaEnvironment.loadedScripts().insert(std::make_pair(path, std::static_pointer_cast<jleLuaScript>(shared_from_this())));
}

void
jleLuaScript::saveToFile()
{
    std::ofstream save{path.getRealPath()};
    save << _sourceCode;
}
