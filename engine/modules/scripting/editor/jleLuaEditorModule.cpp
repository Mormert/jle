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

#include "jleLuaEditorModule.h"
#include <editor/serialization/jleEditorECSRegistration.h>

#include "modules/scripting/components/cLuaScript.h"

void jleLuaEditorModule::initializeECS(jlECS::ECS &ecs) {
    {
        jlECS::ComponentRegistrationConfig config;
        setupDefaultEditorComponentConfig<cLuaScript>(config);
        config.onCreateCallback = [this](jlECS::CreateComponentData& data) {
            onLuaComponentCreated(data);
        };
        config.onDestroyCallback = [this](jlECS::DestroyComponentData& data) {
            onLuaComponentDestroyed(data);
        };
        config.onDuplicateCallback = [this](void* sourceComp, void* destComp) {
            onLuaComponentCopied(static_cast<cLuaScript*>(sourceComp), static_cast<cLuaScript*>(destComp));
        };

        ecs.registerComponentType<cLuaScript>(config);
    }
}

void jleLuaEditorModule::initializeModule(jleSerializationContext &serializationContext) {
    jleLuaModule::initializeModule(serializationContext);

    _luaEnvironment->loadScript(JLE_PATH_HASH("ED:/scripts/editor.lua"), serializationContext);
}

void jleLuaEditorModule::updateEditor(jleSerializationContext &serializationContext) const {
    _luaEnvironment->loadNewlyAddedScripts(serializationContext);
}
