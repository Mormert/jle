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

#include <vector>
#include <memory>

class jleSerializableInterface;
struct jleEngineUpdateContext;
struct jleSerializationContext;
struct jleGameModules;

namespace jlECS
{
class ECS;
}

namespace jleDefaultGameFunctions
{
    std::unique_ptr<jleGameModules> createDefaultModules(bool gameRunning);
    void defaultModulesInitialize(jleGameModules& modules, jlECS::ECS& ecs, jleSerializationContext& serializationContext);
    void defaultModulesUpdate(jleGameModules& modules, jleEngineUpdateContext& ctx, jlECS::ECS& ecs);
    void defaultModulesUpdateRenderablesOnly(jleGameModules& modules, jleEngineUpdateContext& ctx, jlECS::ECS& ecs);
    void defaultPopulateSerializeableInterfaces(jleGameModules &modules, std::vector<jleSerializableInterface *> &interfaces);
}
