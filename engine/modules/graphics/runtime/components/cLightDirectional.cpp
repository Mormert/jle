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

//#include "editor/jleEditor.h"
//#include "editor/jleEditorGizmos.h"

#include "modules/graphics/jle3DSettings.h"
#include "modules/graphics/jleFramePacket.h"


#include "core/serialization/jleBinaryArchive.h"
#include "core/serialization/jleJSONArchive.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cLightDirectional)

void
cLightDirectional::update(jleEngineUpdateContext &ctx)
{
    ctx.currentFramePacket.settings.useDirectionalLight = true;
    auto mat4 = getTransform().getWorldMatrix();

    ctx.currentFramePacket.settings.setDirectionalLight(mat4, _color);
}

void
cLightDirectional::ecsUpdate(jleFramePacket &packet)
{
    packet.settings.useDirectionalLight = true;
    auto mat4 = getTransform().getWorldMatrix();

    packet.settings.setDirectionalLight(mat4, _color);
}

void
cLightDirectional::editorGizmosRender(jleFramePacket &renderGraph, jleEditorGizmos &gizmos)
{
/*#if JLE_BUILD_EDITOR
    auto mesh = gizmos.sunMesh();
    std::shared_ptr<jleMaterial> material = gizmos.sunMaterial();
    renderGraph.sendMesh(mesh, material, getTransform().getWorldMatrix(), _attachedToObject->instanceID(), false);
#endif // JLE_BUILD_EDITOR
 */
}

void
cLightDirectional::registerLua(sol::state &lua)
{
    lua.new_usertype<cLightDirectional>("cLightDirectional", sol::base_classes, sol::bases<cLight>());
}

template <class Archive>
void
cLightDirectional::serialize(Archive &ar)
{
    ar(cereal::base_class<cLight>(this));
}
