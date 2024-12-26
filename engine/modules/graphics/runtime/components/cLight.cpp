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
#include <modules/core/components/cTransform.h>

/*
void
cLight::update(jleEngineUpdateContext &ctx, cTransform& transform)
{
    ctx.currentFramePacket.sendLight(getTransform().getWorldPosition(), _color);
}*/

void
cLight::ecsUpdate(jleFramePacket &packet, const cTransform& transform)
{
    packet.sendLight(transform.getPosition(), _color);
}

/*
void
cLight::editorGizmosRender(jleFramePacket &packet, jleEditorGizmos& gizmos)
{
#if JLE_BUILD_EDITOR
    auto mesh = gizmos.lightLampMesh();
    std::shared_ptr<jleMaterial> material = gizmos.lampMaterial();
    auto matrix = glm::translate(glm::mat4{1.0f}, _attachedToObject->getTransform().getWorldPosition());
    renderGraph.sendMesh(mesh, material, matrix, _attachedToObject->instanceID(), false);
#endif // JLE_BUILD_EDITOR

}
 */

void
cLight::registerLua(sol::state &lua)
{
    // ?
    //lua.new_usertype<cLight>("cLight", sol::call_constructor, sol::no_constructor, "color", &cLight::_color);
}

