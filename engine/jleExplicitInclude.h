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

//#include "cAseprite.h"
#include "cCamera.h"
#include "cCameraFPV.h"
#include "cLight.h"
#include "cLightDirectional.h"
#include "cMesh.h"
#include "cSkybox.h"
#include "jleSceneServer.h"
#include "jleSceneClient.h"
//#include "cSprite.h"
//#include "cSpriteDepth.h"
//#include "cSpritesheet.h"
//#include "cSpritesheetDepth.h"
#include "cText.h"
#include "cUITransformUpdater.h"
#include "jleTransform.h"
#include "cRigidbody.h"
#include "cLuaScript.h"
#include "cAnimator.h"
#include "cSkinnedMesh.h"
#include "cTransformNetSync.h"