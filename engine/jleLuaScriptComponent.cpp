// Copyright (c) 2023. Johan Lind

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
            for (auto &c : ownerObject->customComponents()) {
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
        lua.script(_sourceCode);

        auto setup = lua[_luaScriptName]["setup"];
        auto start = lua[_luaScriptName]["start"];
        auto update = lua[_luaScriptName]["update"];
        auto onDestroy = lua[_luaScriptName]["onDestroy"];

        if (!setup.valid() || !setup.is<sol::function>()) {
            LOGE << "Expected " + _luaScriptName + ".setup function was not found!";
            faultyState = true;
            return;
        }

        if (!start.valid() || !start.is<sol::function>()) {
            LOGE << "Expected " + _luaScriptName + ".start function was not found!";
            faultyState = true;
            return;
        }

        if (!update.valid() || !update.is<sol::function>()) {
            LOGE << "Expected " + _luaScriptName + ".update function was not found!";
            faultyState = true;
            return;
        }

        if (!onDestroy.valid() || !onDestroy.is<sol::function>()) {
            LOGE << "Expected " + _luaScriptName + ".onDestroy function was not found!";
            faultyState = true;
            return;
        }

        _setupLua = setup;
        _startLua = start;
        _updateLua = update;
        _onDestroyLua = onDestroy;

        faultyState = false;

        _luaEnvironment->loadedScripts().insert(std::make_pair(jlePath{filepath, false}, shared_from_this()));

    } catch (std::exception &e) {
        LOGE << "Loading script failed: " << e.what();
        faultyState = true;
    }
}
