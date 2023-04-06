// Copyright (c) 2023. Johan Lind

#include "cSkybox.h"
#include "jle3DRenderer.h"
#include "jleCore.h"
#include "jleRendering.h"
#include "jleResource.h"

cSkybox::cSkybox(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void
cSkybox::start()
{
    if (!_skyboxPath.empty()) {
        _skybox = gCore->resources().loadResourceFromFile<jleSkybox>(jlePath{_skyboxPath});
        if (_skybox) {
            gCore->rendering().rendering3d().setSkybox(_skybox);
        }
    }
}

void
cSkybox::update(float dt)
{
}
