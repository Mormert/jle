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

#include "cTransformNetSync.h"

void
cTransformNetSync::netSyncOut(jleBinaryOutputArchive &ar)
{
    auto &m = object()->getTransform().getLocalMatrix();
    ar(m);
}

void
cTransformNetSync::netSyncIn(jleBinaryInputArchive &ar)
{
    glm::mat4 m;
    ar(m);
    object()->getTransform().setLocalMatrix(m);
}

void
cTransformNetSync::serverUpdate(jleEngineModulesContext& ctx)
{
    jleSerializationContext serializationContext{&ctx.resourcesModule, &ctx.luaEnvironment};
    syncServerToClient(serializationContext);
}
