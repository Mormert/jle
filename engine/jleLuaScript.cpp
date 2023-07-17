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

    if (!gEngine->isGameKilled()) {
        auto lua = gEngine->gameRef().lua();
        if (lua) {
            _currentGameLua = lua;
            loadScript();
        }
    }

    return jleLoadFromFileSuccessCode::SUCCESS;
}

void
jleLuaScript::saveToFile()
{
    std::ofstream save{filepath};
    save << _sourceCode;
}

std::shared_ptr<sol::state> &
jleLuaScript::startLua(sol::table &self)
{

    if (_currentGameLua != gEngine->gameRef().lua()) {
        _currentGameLua = gEngine->gameRef().lua();
        loadScript();
    }

    try {
        self = _setupLua();
    } catch (std::exception &e) {
        LOGE << "Failed to setup lua script: " << e.what();
    }

    try {
        _startLua(self);
    } catch (std::exception &e) {
        LOGE << "Failed to start lua script: " << e.what();
    }

    return _currentGameLua;
}

void
jleLuaScript::updateLua(sol::table &self, float dt)
{
    _updateLua(self, dt);
}

void
jleLuaScript::onDestroyLua(sol::table &self)
{
    _onDestroyLua(self);
}

void
jleLuaScript::loadScript()
{
    auto &lua = *_currentGameLua;

    try {

        lua.new_usertype<jleObject>("jleObject", "name", &jleObject::_instanceName);

        lua.new_usertype<glm::vec3>("vec3", "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z);
        lua.new_usertype<glm::vec4>(
            "vec4", "x", &glm::vec4::x, "y", &glm::vec4::y, "z", &glm::vec4::z, "w", &glm::vec4::w);

        lua.script(_sourceCode);

        _setupLua = lua["test"]["setup"];
        _startLua = lua["test"]["start"];
        _updateLua = lua["test"]["update"];
        _onDestroyLua = lua["test"]["onDestroy"];

    } catch (std::exception &e) {
        LOGE << "Loading script failed: " << e.what();
    }
}
