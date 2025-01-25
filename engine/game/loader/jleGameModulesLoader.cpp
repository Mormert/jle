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

#include "jleGameModulesLoader.h"

#include <modules/graphics/jleGraphicsModule.h>
#include <modules/hierarchy/jleHierarchyModule.h>
#include <modules/physics/jlePhysicsModule.h>

std::unique_ptr<jleGameModules>
jleModuleLoading::createDefaultModules()
{
    auto modules = std::make_unique<jleGameModules>();

    modules->hierarchyModule = std::make_unique<jleHierarchyModule>();
    modules->graphicsModule = std::make_unique<jleGraphicsModule>();
    modules->physicsModule = std::make_unique<jlePhysicsModule>();
    modules->luaModule = std::make_unique<jleLuaModule>();

    return modules;
}
