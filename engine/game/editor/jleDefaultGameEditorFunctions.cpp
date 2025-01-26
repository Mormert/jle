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

#include "jleDefaultGameEditorFunctions.h"

#include "modules/windowing/jleWindowModule.h"
#include "modules/windowing/editor/jleWindowModuleEditor.h"

#include "modules/input/editor/jleInputModuleEditor.h"
#include <modules/graphics/editor/jleGraphicsModuleEditor.h>
#include <modules/hierarchy/editor/jleHierarchyModuleEditor.h>
#include <modules/physics/editor/jlePhysicsModuleEditor.h>
#include <modules/scripting/editor/jleLuaEditorModule.h>

std::unique_ptr<jleGameModules>
jleDefaultGameEditorFunctions::createDefaultModules_Editor(bool gameRunning)
{
    auto modules = std::make_unique<jleGameModules>();

    if (gameRunning) {
        modules->addModule<jleWindowModuleEditor, jleWindowModuleBase>(std::make_unique<jleWindowModuleEditor>());
        modules->addModule<jleInputModuleEditor, jleInputModuleBase>(std::make_unique<jleInputModuleEditor>());
    }else {
        // Add dummy modules for editor mode
        modules->addModule<jleWindowModuleBase>(nullptr);
        modules->addModule<jleInputModuleBase>(nullptr);
    }

    modules->addModule<jleHierarchyModuleEditor, jleHierarchyModule>(std::make_unique<jleHierarchyModuleEditor>());
    modules->addModule<jleGraphicsModuleEditor, jleGraphicsModule>(std::make_unique<jleGraphicsModuleEditor>());
    modules->addModule<jlePhysicsModuleEditor, jlePhysicsModule>(std::make_unique<jlePhysicsModuleEditor>());
    modules->addModule<jleLuaEditorModule, jleLuaModule>(std::make_unique<jleLuaEditorModule>(gameRunning));

    return modules;
}

void
jleDefaultGameEditorFunctions::updateEditorGameModules(jleEditorUpdateContext &ctx)
{
    jleGameModules& modules = ctx.getCurrentModules();

    const std::vector<glm::mat4>& worldMatrices = modules.getModule<jleHierarchyModule>()->getWorldMatrices();

    if (auto* graphicsEditorModule = modules.getModule<jleGraphicsModuleEditor>()){
        graphicsEditorModule->updateEditor(ctx, worldMatrices);
    }

    if (auto* physicsEditorModule = modules.getModule<jlePhysicsModuleEditor>()){
        physicsEditorModule->updateEditor(ctx.editorFramePacket);
    }

    if (auto* luaEditorModule = modules.getModule<jleLuaEditorModule>()){
        luaEditorModule->updateEditor(ctx.engineUpdateContext.serializationContext);
    }
}