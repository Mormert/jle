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

#include "editor/jleEditor.h"
#include "editor/jleEditorGizmos.h"

#include "jle3DGraph.h"
#include "jleGameEngine.h"

#include <glm/gtc/matrix_transform.hpp>

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cLight)

void
cLight::start()
{
}
void
cLight::update(float dt)
{
    gEngine->renderGraph().sendLight(getTransform().getWorldPosition(), _color);
}

void
cLight::editorUpdate(float dt)
{
    update(dt);
}

void
cLight::editorGizmosRender(bool selected)
{
#if JLE_BUILD_EDITOR
    auto mesh = gEditor->gizmos().lightLampMesh();
    std::shared_ptr<jleMaterial> material = gEditor->gizmos().lampMaterial();
    auto matrix = glm::translate(glm::mat4{1.0f}, _attachedToObject->getTransform().getWorldPosition());
    gEngine->renderGraph().sendMesh(mesh, material, matrix, _attachedToObject->instanceID(), false);
#endif // JLE_BUILD_EDITOR
}

void
cLight::registerLua(sol::state &lua, sol::table &table)
{
    lua.new_usertype<cLight>("cLight", sol::base_classes, sol::bases<jleComponent>(), "color", &cLight::_color);
}
