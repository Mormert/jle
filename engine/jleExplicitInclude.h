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

#include "jleBuildConfig.h"

// This header file is used to enforce inclusion of object and components, to
// make sure that they are registered and can be seen in the editor. It needs to
// be included from something that is compiled and used in the engine.

#include "modules/animation/components/cAnimator.h"
#include "modules/graphics/runtime/components/cCamera.h"
#include "modules/graphics/runtime/components/cLight.h"
#include "modules/graphics/runtime/components/cLightDirectional.h"
#include "modules/mesh/components/cMesh.h"
#include "modules/graphics/runtime/components/cSkinnedMesh.h"
#include "modules/graphics/runtime/components/cSkybox.h"
#include "modules/networking/components/cTransformNetSync.h"
#include "modules/networking/jleSceneClient.h"
#include "modules/networking/jleSceneServer.h"
#include "modules/physics/components/cRigidbody.h"
#include "modules/scripting/components/cLuaScript.h"