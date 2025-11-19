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

#include "cLightDirectional.h"

#include "modules/graphics/jleFramePacket.h"
#include <modules/hierarchy/components/cTransform.h>

void
cLightDirectional::ecsUpdate(jleFramePacket &packet, const glm::mat4& worldMatrix)
{
    packet.settings.useDirectionalLight = true;
    packet.settings.setDirectionalLight(worldMatrix, _color);
}

void
cLightDirectional::registerLua(sol::state &lua)
{
    // ?
   // lua.new_usertype<cLightDirectional>("cLightDirectional", sol::base_classes, sol::bases<cLight>());
}
