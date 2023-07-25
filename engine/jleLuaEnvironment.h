// Copyright (c) 2023. Johan Lind

#pragma once

#include "jlePath.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol2/sol.hpp>

class jleLuaScript;

class jleLuaEnvironment
{
public:

    jleLuaEnvironment();

    void setupLua(sol::state& lua);

    void setupLuaGLM(sol::state& lua);

    void loadScript(const jlePath& path);

    [[nodiscard]] sol::state& getState();

private:
    std::vector<std::shared_ptr<jleLuaScript>> _loadedScripts;
    sol::state _luaState;
};
