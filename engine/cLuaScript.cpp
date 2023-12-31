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

#ifdef JLE_BUILD_EDITOR
#include <ImGui/imgui.h>
#include "editor/jleImGuiExtensions.h"
#include "editor/jleEditor.h"
#endif

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cLuaScript)


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
            LOGE << "Failed to load specialization script for " << _attachedToObject->instanceName() << ": "
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
#ifdef JLE_BUILD_EDITOR
    if (!gEngine->isGameKilled()) {

        ImGui::BeginGroupPanel("Lua Variables");

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

        ImGui::EndGroupPanel();

    }else
    {
        ImGui::Text("Start the game to see Lua variables");
    }
#endif
}
