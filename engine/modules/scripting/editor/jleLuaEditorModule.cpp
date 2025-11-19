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

    jlECS::ComponentRegistrationConfig config;
    config.module = this;

    setupDefaultEditorComponentConfig<cLuaScript>(config);

    if(_gameRunning)
    {
        config.onCreateCallback = [this](jlECS::CreateComponentData& data) {
            onLuaComponentCreated(data);
        };
        config.onDestroyCallback = [this](jlECS::DestroyComponentData& data) {
            onLuaComponentDestroyed(data);
        };
        config.onDuplicateCallback = [this](void* sourceComp, void* destComp) {
            onLuaComponentCopied(static_cast<cLuaScript*>(sourceComp), static_cast<cLuaScript*>(destComp));
        };
    }
    else
    {
        config.serializeImGuiF = editorSerializeLuaScript;

        config.onDuplicateCallback = [this](void* sourceComp, void* destComp) {
            onLuaComponentCopied(static_cast<cLuaScript*>(sourceComp), static_cast<cLuaScript*>(destComp));
        };
    }

    ecs.registerComponentType<cLuaScript>(config);

}

void jleLuaEditorModule::initializeModule(jleSerializationContext &serializationContext) {
    _luaEnvironment = std::make_unique<jleLuaEnvironment>(true);

    jleLuaModule::initializeModule(serializationContext);

    _luaEnvironment->loadScript(JLE_PATH_HASH("ED:/scripts/editor.lua"), serializationContext);
}

void jleLuaEditorModule::updateEditor(jleSerializationContext &serializationContext) const {
    ZoneScoped;
    _luaEnvironment->loadNewlyAddedScripts(serializationContext);
}

void
jleLuaEditorModule::editorSerializeLuaScript(jlECS::ComponentContainer *cc,
                                             jleImGuiArchive &archive,
                                             int componentIndex,
                                             int objectIndex)
{
    auto* module = static_cast<jleLuaEditorModule *>(cc->getModule());
    auto* luaEnv = &module->getEnvironment();
    auto* scriptComponent = cc->getPtr<cLuaScript>(componentIndex);

    scriptComponent->_luaComponent.baseClass = "LuaComponent";
    archive(CEREAL_NVP(scriptComponent->_luaComponent));

    jleLuaClass* luaClass = luaEnv->getLuaClassPtr(scriptComponent->_luaComponent.luaClassName);
    if(luaClass)
    {
        luaClass->serializeClass(archive, scriptComponent->_self);
    }
}
