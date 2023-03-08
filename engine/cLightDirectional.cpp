// Copyright (c) 2023. Johan Lind

#include "cLightDirectional.h"

#include "jleCore.h"
#include "jleRendering.h"

cLightDirectional::cLightDirectional(jleObject *owner, jleScene *scene) : cLight(owner, scene) {}

void
cLightDirectional::update(float dt)
{
    gCore->rendering().rendering3d().enableDirectionalLight();
    auto mat4 = getTransform().getWorldMatrix();

    gCore->rendering().rendering3d().setDirectionalLight(mat4, _color);
}
