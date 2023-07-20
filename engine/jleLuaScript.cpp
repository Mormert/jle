// Copyright (c) 2023. Johan Lind

#include "jleLuaScript.h"
#include "jleGameEngine.h"

jleLoadFromFileSuccessCode
jleLuaScript::loadFromFile(const jlePath &path)
{
    std::ifstream load{path.getRealPath()};
    if (!load.good()) {
        return jleLoadFromFileSuccessCode::FAIL;
    }

    std::stringstream buffer;
    buffer << load.rdbuf();

    _sourceCode = buffer.str();
    _luaScriptName = path.getFileNameNoEnding();

    loadScript();

    return jleLoadFromFileSuccessCode::SUCCESS;
}

void
jleLuaScript::saveToFile()
{
    std::ofstream save{filepath};
    save << _sourceCode;
}

std::shared_ptr<sol::state> &
jleLuaScript::setupLua(sol::table &self, jleObject *ownerObject)
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
    return _currentGameLua;
}

void
jleLuaScript::startLua(sol::table &self)
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
jleLuaScript::updateLua(sol::table &self, float dt)
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
jleLuaScript::onDestroyLua(sol::table &self)
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
jleLuaScript::loadScript()
{
    if (gEngine->isGameKilled()) {
        return;
    }

    auto gameLua = gEngine->gameRef().lua();

    if (_currentGameLua != gameLua) {
        _setupLua = {};
        _startLua = {};
        _updateLua = {};
        _onDestroyLua = {};
        _currentGameLua = gameLua;
    }

    auto &lua = *_currentGameLua;

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

    } catch (std::exception &e) {
        LOGE << "Loading script failed: " << e.what();
        faultyState = true;
    }
}
