// Copyright (c) 2022. Johan Lind

#include "cSkybox.h"
#include "jle3DRenderer.h"
#include "jleCore.h"
#include "jleResource.h"

cSkybox::cSkybox(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void
cSkybox::start()
{
    if (!_skyboxPath.empty()) {
        _skybox = gCore->resources().loadResourceFromFile<jleSkybox>(jleRelativePath{_skyboxPath});
        if (_skybox) {
            gCore->rendering().rendering3d().setSkybox(_skybox);
        }
    }
}

void
cSkybox::update(float dt)
{
}

void
cSkybox::toJson(nlohmann::json &j_out)
{
    j_out["skyboxPath"] = _skyboxPath;
}

void
cSkybox::fromJson(const nlohmann::json &j_in)
{
    _skyboxPath = j_in["skyboxPath"];
    if (!_skyboxPath.empty()) {
        _skybox = gCore->resources().loadResourceFromFile<jleSkybox>(jleRelativePath{_skyboxPath});
        if (_skybox) {
            gCore->rendering().rendering3d().setSkybox(_skybox);
        }
    }
}