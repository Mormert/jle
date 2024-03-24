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
#include "jleLuaEnvironment.h"

#if JLE_BUILD_IMGUI
#include <ImGui/imgui.h>
#endif

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cLuaScript)

void
cLuaScript::start()
{
    if (!_isInitialized) {
        initializeLuaComponent();
    }
}

void
cLuaScript::update(float dt)
{
    const auto luaClass = gEngine->luaEnvironment()->getState()[_luaClass.luaClassName];

    try {
        sol::protected_function updateFunc = luaClass["update"];
        updateFunc(_self, dt);
    } catch (std::exception &e) {
        LOGE << "Error running lua update: " << e.what();
    }
}

void
cLuaScript::onDestroy()
{
    const auto luaClass = gEngine->luaEnvironment()->getState()[_luaClass.luaClassName];

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
cLuaScript::initializeLuaComponent()
{
    const auto luaClass = gEngine->luaEnvironment()->getState()[_luaClass.luaClassName];

    gEngine->luaEnvironment()->loadedLuaClasses()[_luaClass.luaClassName];

    try{
        sol::protected_function classConstructor = luaClass["new"];
        if (classConstructor.valid()) {
            _self = classConstructor(luaClass);
            _self["object"] = object();

            _isInitialized = true;
        } else {
            LOGE << "Failed to initialize Lua class component on cLuaScript " << _luaClass.luaClassName;
            _isInitialized = false;
        }
    }catch(std::exception&e){
        LOGE << "Failed to initialize Lua class component on cLuaScript " << _luaClass.luaClassName << " reason: " << e.what();
        _isInitialized = false;
    }

}

template <class Archive>
void
cLuaScript::serialize(Archive &ar)
{
    try {
        ar(CEREAL_NVP(_luaClass));

        if (!_isInitialized) {
            initializeLuaComponent();
        }

        auto it = gEngine->luaEnvironment()->loadedLuaClasses().find(_luaClass.luaClassName);
        if (it != gEngine->luaEnvironment()->loadedLuaClasses().end()) {
            it->second.serializeClass(ar, _self);
        }
    } catch (std::exception &e) {
        LOGE << "Failed to serialize cLuaScript";
    }
}
