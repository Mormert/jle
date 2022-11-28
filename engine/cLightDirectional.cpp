// Copyright (c) 2022. Johan Lind

#include "cLightDirectional.h"

#include "jleCore.h"

cLightDirectional::cLightDirectional(jleObject *owner, jleScene *scene) : cLight(owner, scene) {}

void
cLightDirectional::update(float dt)
{
    gCore->rendering().rendering3d().enableDirectionalLight();
    auto mat4 = _transform->getWorldMatrix();

    gCore->rendering().rendering3d().setDirectionalLight(mat4, _color);
}
