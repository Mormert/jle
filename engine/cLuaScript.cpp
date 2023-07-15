// Copyright (c) 2023. Johan Lind

#include "cLuaScript.h"

cLuaScript::cLuaScript(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void
cLuaScript::start()
{
    _lua = std::make_shared<sol::state>();

    _lua->open_libraries(sol::lib::base,
                         sol::lib::math,
                         sol::lib::string,
                         sol::lib::coroutine,
                         sol::lib::package,
                         sol::lib::debug,
                         sol::lib::io,
                         sol::lib::table,
                         sol::lib::os);

    _lua->set_function("LOGE", [](std::string a) { LOGE << a; });

    try {
        _lua->script_file(_scriptPath.getRealPath());
    } catch (std::exception &e) {
        LOGE << "Error running lua script:" << e.what();
    }
}

void
cLuaScript::update(float dt)
{
}
