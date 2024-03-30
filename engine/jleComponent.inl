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

#include "jleComponent.h"
#include "jleObject.h"

template <typename T>
void
jleComponent::registerLuaComponentFunctions_Impl(sol::usertype<jleObject> &luaObjType)
{
    std::string addComp = "addComponent_" + std::string{componentName()};
    luaObjType.set_function(addComp.c_str(), [](jleObject &o) { return o.addComponent<T>(); });

    std::string getComp = "getComponent_" + std::string{componentName()};
    luaObjType.set_function(getComp.c_str(), [](jleObject &o) { return o.getComponent<T>(); });
}