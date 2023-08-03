// Copyright (c) 2023. Johan Lind

#include <editor/jleEditor.h>
#include "cLightDirectional.h"

#include "jleCore.h"
#include "jleRendering.h"

cLightDirectional::cLightDirectional(jleObject *owner, jleScene *scene) : cLight(owner, scene) {}

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cLightDirectional)

void
cLightDirectional::update(float dt)
{
    gCore->renderSettings().useDirectionalLight = true;
    auto mat4 = getTransform().getWorldMatrix();

    gCore->renderSettings().setDirectionalLight(mat4, _color);
}


void
cLightDirectional::editorGizmosRender(bool selected)
{
#ifdef BUILD_EDITOR
    auto mesh = gEditor->directionalLightLampGizmoMesh.get();
    std::shared_ptr<jleMaterial> material{};
    gEngine->renderGraph().sendMesh(mesh, material, getTransform().getWorldMatrix(), _attachedToObject->instanceID(), false);
#endif // BUILD_EDITOR
}

void
cLightDirectional::registerLua(sol::state &lua, sol::table &table)
{

}
