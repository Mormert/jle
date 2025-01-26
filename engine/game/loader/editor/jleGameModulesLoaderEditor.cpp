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


#include "jleGameModulesLoaderEditor.h"

#include <modules/graphics/editor/jleGraphicsModuleEditor.h>
#include <modules/hierarchy/editor/jleHierarchyModuleEditor.h>
#include <modules/physics/editor/jlePhysicsModuleEditor.h>
#include <modules/scripting/editor/jleLuaEditorModule.h>

std::unique_ptr<jleGameModules>
jleModuleLoading::createDefaultModules_Editor(bool gameRunning)
{
    auto modules = std::make_unique<jleGameModules>();

    modules->addModule<jleHierarchyModuleEditor, jleHierarchyModule>(std::make_unique<jleHierarchyModuleEditor>());
    modules->addModule<jleGraphicsModuleEditor, jleGraphicsModule>(std::make_unique<jleGraphicsModuleEditor>());
    modules->addModule<jlePhysicsModuleEditor, jlePhysicsModule>(std::make_unique<jlePhysicsModuleEditor>());
    modules->addModule<jleLuaEditorModule, jleLuaModule>(std::make_unique<jleLuaEditorModule>(gameRunning));

    //modules->hierarchyModule = std::make_unique<jleHierarchyModuleEditor>();
    //modules->graphicsModule = std::make_unique<jleGraphicsModuleEditor>();
    //modules->physicsModule = std::make_unique<jlePhysicsModuleEditor>();
    //modules->luaModule = std::make_unique<jleLuaEditorModule>(gameRunning);

    return modules;
}