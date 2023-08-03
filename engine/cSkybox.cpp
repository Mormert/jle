// Copyright (c) 2023. Johan Lind

#include "cSkybox.h"
#include "jle3DRenderer.h"
#include "jleGameEngine.h"

#include "jleResource.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cSkybox)


cSkybox::cSkybox(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void
cSkybox::start()
{
}

void
cSkybox::update(float dt)
{
}
