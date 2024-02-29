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

#include "jleLuaScriptComponent.h"
#include "jleLuaEnvironment.h"
#include "jleObject.h"


void
jleLuaScriptComponent::setupLua(sol::table &self, jleObject *ownerObject)
{
    loadScript();

    if (!faultyState) {
        auto result = _setupLua();
        if (result.valid()) {
            self = result;
            self["object"] = ownerObject;
            for (auto &c : ownerObject->components()) {
                c->registerSelfLua(self);
            }
        } else {
            sol::error err = result;
            LOGE << "Failed starting script: " << err.what();
            faultyState = true;
        }
    }
}

void
jleLuaScriptComponent::startLua(sol::table &self)
{
    if (faultyState) {
        return;
    }
    auto result = _startLua(self);
    if (!result.valid()) {
        sol::error err = result;
        LOGE << "Failed starting script: " << err.what();
        faultyState = true;
    }
}
void
jleLuaScriptComponent::updateLua(sol::table &self, float dt)
{
    if (faultyState) {
        return;
    }

    auto result = _updateLua(self, dt);
    if (!result.valid()) {
        sol::error err = result;
        LOGE << "Failed updating script: " << err.what();
    }
}
void
jleLuaScriptComponent::onDestroyLua(sol::table &self)
{
    if (faultyState) {
        return;
    }
    auto result = _onDestroyLua(self);
    if (!result.valid()) {
        sol::error err = result;
        LOGE << "Failed destroying script: " << err.what();
    }
}
void
jleLuaScriptComponent::loadScript()
{
    if (gEngine->isGameKilled()) {
        return;
    }

    auto &lua = _luaEnvironment->getState();

    try {
        const auto absoluteSrcCodePath = path.getRealPath();
        lua.script(_sourceCode, absoluteSrcCodePath);

        auto setup = lua[_luaScriptName]["setup"];
        auto start = lua[_luaScriptName]["start"];
        auto update = lua[_luaScriptName]["update"];
        auto onDestroy = lua[_luaScriptName]["onDestroy"];

        if (!setup.valid() || !setup.is<sol::function>()) {
            LOGE << "Expected " + _luaScriptName + ".setup() function was not found!";
            faultyState = true;
            return;
        }

        if (!start.valid() || !start.is<sol::function>()) {
            LOGE << "Expected " + _luaScriptName + ".start(self) function was not found!";
            faultyState = true;
            return;
        }

        if (!update.valid() || !update.is<sol::function>()) {
            LOGE << "Expected " + _luaScriptName + ".update(self, dt) function was not found!";
            faultyState = true;
            return;
        }

        if (!onDestroy.valid() || !onDestroy.is<sol::function>()) {
            LOGE << "Expected " + _luaScriptName + ".onDestroy(self) function was not found!";
            faultyState = true;
            return;
        }

        _setupLua = setup;
        _startLua = start;
        _updateLua = update;
        _onDestroyLua = onDestroy;

        faultyState = false;

        _luaEnvironment->loadedScripts().insert(std::make_pair(path, shared_from_this()));

    } catch (std::exception &e) {
        LOGE << "Loading script failed: " << e.what();
        faultyState = true;
    }
}
