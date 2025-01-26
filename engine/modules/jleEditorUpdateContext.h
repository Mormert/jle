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

#include "jleEngineUpdateContext.h"

class jleEditor;
class jleResourceIndexer;
class jleEditorGizmos;
class jleGameModules;

namespace jlECS
{
class ECS;
}

class jleEditorUpdateContext
{
public:
    jleEngineUpdateContext &engineUpdateContext;
    jleResourceIndexer &resourceIndexer;
    jleEditorGizmos &gizmos;
    jleFramePacket &editorFramePacket;
    jlECS::ECS &editorEcs;
    jleGameModules &editorGameModules;

    jlECS::ECS &getCurrentECS();
    jleGameModules &getCurrentModules();
};
