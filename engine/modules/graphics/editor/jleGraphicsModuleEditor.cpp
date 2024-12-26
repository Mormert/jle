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

#include "jleGraphicsModuleEditor.h"

#include <editor/serialization/jleEditorECSRegistration.h>

#include "modules/graphics/runtime/components/cCamera.h"
#include "modules/graphics/runtime/components/cLight.h"
#include "modules/graphics/runtime/components/cLightDirectional.h"
#include "modules/graphics/runtime/components/cMesh.h"
#include "modules/graphics/runtime/components/cSkinnedMesh.h"
#include "modules/graphics/runtime/components/cSkybox.h"


void
jleGraphicsModuleEditor::initializeECS(jlECS::ECS &ecs)
{
    registerEditorECSComponent<cCamera>(ecs);
    registerEditorECSComponent<cLight>(ecs);
    registerEditorECSComponent<cLightDirectional>(ecs);
    registerEditorECSComponent<cMesh>(ecs);
    registerEditorECSComponent<cSkinnedMesh>(ecs);
    registerEditorECSComponent<cSkybox>(ecs);
}
