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
    void
    serialize(Archive &ar){
        try {
            jleSerializationContext &ctx = ar.ctx;
            jleAssert(ctx.get<jleLuaEnvironment>());
            auto &luaEnv = *ctx.get<jleLuaEnvironment>();

            ar(CEREAL_NVP(_luaClass));

            if (!_isInitialized && !_luaClass.luaClassName.empty()) {
                initializeLuaComponent(luaEnv);
            }

            auto it = luaEnv.loadedLuaClasses().find(_luaClass.luaClassName);
            if (it != luaEnv.loadedLuaClasses().end()) {
                it->second.serializeClass(ar, _self);
            }else {
                _isInitialized = false;
            }
        } catch (std::exception &e) {
            LOGE << "Failed to serialize cLuaScript";
        }
    }

    void start(jleLuaEnvironment& environment);

    void update(jleLuaEnvironment& environment, float dt);

    void onDestroy(jleLuaEnvironment& environment);

    sol::table &getSelf();

private:
    void initializeLuaComponent(jleLuaEnvironment &luaEnvironment);
    bool _isInitialized{false};

    jleLuaClassSerialization _luaClass{};
    sol::table _self;

    friend class jleLuaModule;
};
