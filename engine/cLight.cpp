// Copyright (c) 2023. Johan Lind

#include <editor/jleEditor.h>
#include "cLight.h"

#include "jleCore.h"
#include "jleRendering.h"

cLight::cLight(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}
void
cLight::start()
{
}
void
cLight::update(float dt)
{
    gCore->rendering().rendering3d().sendLight(getTransform().getWorldPosition(), _color);
}

void
cLight::editorUpdate(float dt)
{
    update(dt);
}

void
cLight::editorGizmosRender(bool selected)
{
    auto mesh = gEditor->pointLightLampGizmoMesh.get();
    std::shared_ptr<jleMaterial> material{};
    auto matrix = glm::translate(glm::mat4{1.0f}, _attachedToObject->getTransform().getWorldPosition());
    gEngine->rendering().rendering3d().sendMesh(mesh, material, matrix, _attachedToObject->instanceID(), false);
}
