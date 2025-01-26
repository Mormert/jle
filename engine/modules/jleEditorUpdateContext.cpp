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

#include "jleEditorUpdateContext.h"
#include "game/jleGameRuntime.h"
#include "game/jleGame.h"

jlECS::ECS &
jleEditorUpdateContext::getCurrentECS()
{
    return engineUpdateContext.gameRuntime.isGameKilled() ? editorEcs : engineUpdateContext.gameRuntime.getGame().getECS();
}

jleGameModules &
jleEditorUpdateContext::getCurrentModules()
{
    return engineUpdateContext.gameRuntime.isGameKilled() ? editorGameModules : engineUpdateContext.gameRuntime.getGame().getModules();
}
