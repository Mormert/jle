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

#include "cLight.h"
#include "modules/graphics/jleFramePacket.h"
#include <modules/hierarchy/components/cTransform.h>

void
cLight::ecsUpdate(jleFramePacket &packet, const glm::mat4& worldMatrix)
{
    packet.sendLight(glm::vec3(worldMatrix[3]), _color);
}

void
cLight::registerLua(sol::state &lua)
{
    // ?
    //lua.new_usertype<cLight>("cLight", sol::call_constructor, sol::no_constructor, "color", &cLight::_color);
}

