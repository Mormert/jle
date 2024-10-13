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

#include "core/jleComponent.h"
#include "core/jleResourceRef.h"
#include "modules/scripting/jleLuaClassSerialization.h"
#include "modules/scripting/jleLuaScript.h"

#include "jleGameEngine.h"
#include "modules/scripting/jleLuaEnvironment.h"

class cLuaScript : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cLuaScript)
public:
    cLuaScript() = default;

    // Called when cloned/duplicated
    cLuaScript(const cLuaScript &other);

    template <class Archive>
    void
    serialize(Archive &ar);

    void start(jleEngineModulesContext &ctx) override;

    void update(jleEngineModulesContext &ctx) override;

    void onDestroy(jleEngineModulesContext &ctx) override;

    sol::table &getSelf();

private:
    void initializeLuaComponent(jleLuaEnvironment &luaEnvironment);
    bool _isInitialized{false};

    jleLuaClassSerialization _luaClass;

    jleLuaEnvironment* _luaEnvironment;

    sol::table _self;
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cLuaScript)

CEREAL_REGISTER_TYPE(cLuaScript)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cLuaScript)
