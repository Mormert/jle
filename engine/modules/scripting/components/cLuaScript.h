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

#pragma once

#include "jleBuildConfig.h"

#include <core/jleResourceRef.h>
#include <sol2/sol.hpp>

#include "modules/scripting/jleLuaClassSerialization.h"
#include "modules/scripting/jleLuaScript.h"
#include "modules/scripting/jleLuaEnvironment.h"

class cLuaScript
{
public:
    cLuaScript() = default;

    template <class Archive>
    void serialize(Archive &ar);

    void start(jleLuaEnvironment& environment);

    void update(jleLuaEnvironment& environment, float dt);

    void onDestroy(jleLuaEnvironment& environment);

    sol::table &getSelf();

private:
    void initializeLuaComponent(jleLuaEnvironment &luaEnvironment);
    jleLuaClassSerialization _luaComponent{};

    sol::table _self{};

    bool _isInitialized{false};

    friend class jleLuaModule;
    friend class jleLuaEditorModule;
};

template <class Archive>
void
cLuaScript::serialize(Archive &ar)
{
    try {
        jleSerializationContext &ctx = ar.ctx;

        _luaComponent.baseClass = "LuaComponent";
        ar(CEREAL_NVP(_luaComponent));

        if(auto* luaEnv = ctx.get<jleLuaEnvironment>())
        {
            if (!_isInitialized && !_luaComponent.luaClassName.empty() && !luaEnv->isEditorMode()) {
                initializeLuaComponent(*luaEnv);
            }

            jleLuaClass* luaClass = luaEnv->getLuaClassPtr(_luaComponent.luaClassName);
            if (luaClass != nullptr) {
                if(_self.valid())
                {
                    luaClass->serializeClass(ar, _self);
                }
            }else {
                _isInitialized = false;
            }
        }
    } catch (std::exception &e) {
        LOGE << "Failed to serialize cLuaScript";
    }
}
