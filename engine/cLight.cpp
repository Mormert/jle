// Copyright (c) 2023. Johan Lind

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
