// Copyright (c) 2023. Johan Lind

#include "cLight.h"

#include "editor/jleEditor.h"
#include "editor/jleEditorGizmos.h"

#include "jle3DGraph.h"
#include "jleGameEngine.h"

#include <glm/gtc/matrix_transform.hpp>

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cLight)


cLight::cLight(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}
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
#ifdef BUILD_EDITOR
    auto mesh = gEditor->gizmos().lightLampMesh();
    std::shared_ptr<jleMaterial> material = gEditor->gizmos().lampMaterial();
    auto matrix = glm::translate(glm::mat4{1.0f}, _attachedToObject->getTransform().getWorldPosition());
    gEngine->renderGraph().sendMesh(mesh, material, matrix, _attachedToObject->instanceID(), false);
#endif // BUILD_EDITOR
}

void
cLight::registerLua(sol::state &lua, sol::table &table)
{
    lua.new_usertype<cLight>("cLight", sol::base_classes, sol::bases<jleComponent>(), "color", &cLight::_color);
}
