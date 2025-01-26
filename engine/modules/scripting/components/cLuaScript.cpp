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

#include "cLuaScript.h"
#include "jleGameEngine.h"
#include "modules/scripting/jleLuaEnvironment.h"

void
cLuaScript::start(jleLuaEnvironment& environment)
{
    if (!_isInitialized) {
        return;
    }

    const auto luaClass = environment.getState()[_luaComponent.luaClassName];

    try {
        sol::protected_function startFunc = luaClass["start"];
        startFunc(_self);
    } catch (std::exception &e) {
        LOGE << "Error running lua start: " << e.what();
    }
}

void
cLuaScript::update(jleLuaEnvironment& environment, float dt)
{
    if (!_isInitialized && !_luaComponent.luaClassName.empty()) {
        initializeLuaComponent(environment);
    }

    const auto luaClass = environment.getState()[_luaComponent.luaClassName];

    try {
        sol::protected_function updateFunc = luaClass["update"];
        updateFunc(_self, dt);
    } catch (std::exception &e) {
        LOGE << "Error running lua update: " << e.what();
    }
}

void
cLuaScript::onDestroy(jleLuaEnvironment& environment)
{
    if (!_isInitialized) {
        return;
    }

    const auto luaClass = environment.getState()[_luaComponent.luaClassName];

    try {
        sol::protected_function destroyFunc = luaClass["destroy"];
        destroyFunc(_self);
    } catch (std::exception &e) {
        LOGE << "Error running lua destroy: " << e.what();
    }
}

sol::table &
cLuaScript::getSelf()
{
    return _self;
}

void
cLuaScript::initializeLuaComponent(jleLuaEnvironment& luaEnvironment)
{
    if (!luaEnvironment.getLuaClassPtr(_luaComponent.luaClassName)) {
        _isInitialized = false;
        LOGE << "Failed to load Lua class component on cLuaScript " << _luaComponent.luaClassName;
        return;
    }

    const auto luaClass = luaEnvironment.getState()[_luaComponent.luaClassName];

    try {
        sol::protected_function classConstructor = luaClass["new"];
        if (classConstructor.valid()) {
            _self = classConstructor(luaClass);

            _isInitialized = true;
        } else {
            LOGE << "Failed to initialize Lua class component on cLuaScript " << _luaComponent.luaClassName;
            _isInitialized = false;
        }
    } catch (std::exception &e) {
        LOGE << "Failed to initialize Lua class component on cLuaScript " << _luaComponent.luaClassName << " reason: " << e.what();
        _isInitialized = false;
    }
}
