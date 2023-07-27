// Copyright (c) 2023. Johan Lind

#include "cLuaScript.h"
#include "jleGameEngine.h"

#ifdef BUILD_EDITOR
#include "ImGui/imgui.h"
#include "editor/jleEditor.h"
#endif

cLuaScript::cLuaScript(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void
cLuaScript::start()
{
    if (!_scriptRef) {
        LOGE << "Can't start script since there is a reference issue";
        runUpdate = false;
        return;
    }

    _scriptRef->setupLua(_self, _attachedToObject);

    if (!_specializationScript.empty()) {
        sol::load_result fx = gEngine->luaEnvironment()->getState().load(_specializationScript);
        if (!fx.valid()) {
            sol::error err = fx;
            LOGE << "Failed to load specialization script for " << _attachedToObject->_instanceName << ": "
                 << err.what();
        }

        fx(_self);
    }

    _scriptRef->startLua(_self);
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
        if (_scriptRef) {
            _scriptRef->onDestroyLua(_self);
        }
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
cLuaScript::editorInspectorImGuiRender()
{
#ifdef BUILD_EDITOR
    if (!gEngine->isGameKilled()) {

        ImGui::BeginChild("LuaVars", ImVec2(0,200), true, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Lua Variables");

        try {
            sol::protected_function f = gEditor->luaEnvironment()->getState()["luaEditor"]["tableImGui"];
            if (f.valid()) {
                auto res = f(getSelf());
                if (!res.valid()) {
                    sol::error err = res;
                    ImGui::Text("Lua Error: %s", err.what());
                }
            }
        } catch (std::exception &e) {
            ImGui::Text("Lua Error: %s", e.what());
        }

        ImGui::EndChild();

    }else
    {
        ImGui::Text("Start the game to see Lua variables");
    }
#endif
}
