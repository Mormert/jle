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

#include <core/jleCommon.h>

#include <editor/serialization/jleEditorSerialization.h>
#include "editor/jleImGuiArchive.h"

#include "modules/graphics/runtime/components/cCamera.h"
#include "modules/graphics/runtime/components/cCameraFPV.h"
#include "modules/graphics/runtime/components/cLight.h"
#include "modules/graphics/runtime/components/cLightDirectional.h"
#include "modules/graphics/runtime/components/cMesh.h"
#include "modules/graphics/runtime/components/cSkinnedMesh.h"
#include "modules/graphics/runtime/components/cSkybox.h"


void
jleGraphicsModuleEditor::initializeECS(jlECS::ECSEditor &ecs)
{

}
