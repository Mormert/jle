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

#ifndef JLE_LUASCRIPTCOMPONENT_H
#define JLE_LUASCRIPTCOMPONENT_H

#include "jleCommon.h"

#include "jleLuaScript.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol2/sol.hpp>

class jleLuaScriptComponent : public jleLuaScript
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleLuaScriptComponent, "lua");

    void setupLua(sol::table &self, jleObject *ownerObject);

    void loadScript() override;

    void startLua(sol::table &self);
    void updateLua(sol::table &self, float dt);
    void onDestroyLua(sol::table &self);

private:
    sol::protected_function _setupLua;
    sol::protected_function _startLua;
    sol::protected_function _updateLua;
    sol::protected_function _onDestroyLua;

};

CEREAL_REGISTER_TYPE(jleLuaScriptComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleLuaScript, jleLuaScriptComponent)

#endif // JLE_LUASCRIPTCOMPONENT_H
