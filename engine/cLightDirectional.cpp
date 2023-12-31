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

#include "editor/jleEditor.h"
#include "editor/jleEditorGizmos.h"

#include "jle3DGraph.h"
#include "jle3DSettings.h"

cLightDirectional::cLightDirectional(jleObject *owner, jleScene *scene) : cLight(owner, scene) {}

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cLightDirectional)

void
cLightDirectional::update(float dt)
{
    gEngine->renderSettings().useDirectionalLight = true;
    auto mat4 = getTransform().getWorldMatrix();

    gEngine->renderSettings().setDirectionalLight(mat4, _color);
}


void
cLightDirectional::editorGizmosRender(bool selected)
{
#ifdef JLE_BUILD_EDITOR
    auto mesh = gEditor->gizmos().sunMesh();
    std::shared_ptr<jleMaterial> material = gEditor->gizmos().sunMaterial();
    gEngine->renderGraph().sendMesh(mesh, material, getTransform().getWorldMatrix(), _attachedToObject->instanceID(), false);
#endif // JLE_BUILD_EDITOR
}

void
cLightDirectional::registerLua(sol::state &lua, sol::table &table)
{

}
