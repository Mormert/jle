// Copyright (c) 2023. Johan Lind

#include "cLuaScript.h"
#include "jleGameEngine.h"

cLuaScript::cLuaScript(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

cLuaScript::~cLuaScript()
{
    // First destroy the 'self' object by making it empty, which is referencing the lua::state,
    // which is first needed to avoid a crash. This will not do anything in editor mode, either.
    _self = {};
    _luaKeepAliveRef.reset();
}

void
cLuaScript::start()
{
    if(!_scriptRef)
    {
        LOGE << "Can't start script since there a reference issue";
        return;
    }

    _luaKeepAliveRef = _scriptRef->startLua(_self);
}

void
cLuaScript::update(float dt)
{
    if (runUpdate) {
        try {
            _scriptRef->updateLua(_self, dt);
        } catch (std::exception &e) {
            LOGE << "Error running lua update: " << e.what();
            runUpdate = false;
        }
    }
}

void
cLuaScript::onDestroy()
{
    try {
        _scriptRef->onDestroyLua(_self);
    } catch (std::exception &e) {
        LOGE << "Error running lua destroy: " << e.what();
    }
}
