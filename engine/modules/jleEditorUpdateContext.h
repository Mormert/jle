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

#include "game/jleGameRuntime.h"
#include "game/jleGame.h"

class jleEditorWindow;
class jleFramePacket;
class jleEditor;
class jleResourceIndexer;
class jleEditorGizmos;
struct jleGameModules;

namespace jlECS
{
class ECS;
}

class jleEditorUpdateContext
{
public:
    jleEngineUpdateContext &engineUpdateContext;
    jleResourceIndexer &resourceIndexer;
    jleEditorWindow &editorWindow;
    jleEditorGizmos &gizmos;
    jlECS::ECS &editorEcs;
    jleGameModules &editorGameModules;

    jlECS::ECS &getCurrentECS() const
    {
        return engineUpdateContext.gameRuntime.isGameKilled() ? editorEcs : engineUpdateContext.gameRuntime.getGame().getECS();
    }

    jleGameModules &getCurrentModules() const
    {
        return engineUpdateContext.gameRuntime.isGameKilled() ? editorGameModules : engineUpdateContext.gameRuntime.getGame().getModules();
    }
};
