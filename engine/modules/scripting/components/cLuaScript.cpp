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
        initializeLuaComponent(environment);
    }

    const auto luaClass = environment.getState()[_luaClass.luaClassName];

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
    const auto luaClass = environment.getState()[_luaClass.luaClassName];

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
    const auto luaClass = environment.getState()[_luaClass.luaClassName];

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
    const auto luaClass = luaEnvironment.getState()[_luaClass.luaClassName];

    luaEnvironment.loadedLuaClasses()[_luaClass.luaClassName];

    try {
        sol::protected_function classConstructor = luaClass["new"];
        if (classConstructor.valid()) {
            _self = classConstructor(luaClass);

            _isInitialized = true;
        } else {
            LOGE << "Failed to initialize Lua class component on cLuaScript " << _luaClass.luaClassName;
            _isInitialized = false;
        }
    } catch (std::exception &e) {
        LOGE << "Failed to initialize Lua class component on cLuaScript " << _luaClass.luaClassName
             << " reason: " << e.what();
        _isInitialized = false;
    }
}

namespace
{
void
deep_copy(sol::state &lua, const sol::table &src, sol::table &dest)
{
    if (src.is<sol::table>()) {
        sol::table src_table = src;
        for (auto const &kv_pair : src_table) {
            sol::object key = kv_pair.first;
            sol::object value = kv_pair.second;
            if (value.is<sol::table>()) {
                sol::table nested_dest_table = lua.create_table();
                deep_copy(lua, value, nested_dest_table);
                dest[key] = nested_dest_table;
            } else {
                dest[key] = value;
            }
        }
    }
}
} // namespace

cLuaScript::cLuaScript(const cLuaScript &other)
{

    _luaClass = other._luaClass;
    // TODO implement proper ECS copying here
    // let it crash for now
    assert(false);
    //initializeLuaComponent(nullptr);
    //deep_copy(_luaEnvironment->getState(), other._self, _self);
}

