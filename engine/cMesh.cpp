// Copyright (c) 2023. Johan Lind

#include "cMesh.h"
#include "jleCore.h"
#include "jleResource.h"
#include "jleRendering.h"

cMesh::cMesh(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void
cMesh::start()
{
    //_transform = addDependencyComponentInStart<cTransform>();
}

void
cMesh::update(float dt)
{
    if (_mesh) {
        gCore->rendering().rendering3d().sendMesh(
            _mesh, _material, getTransform().getWorldMatrix(), _attachedToObject->instanceID());
    }

    if (_meshRef) {
        gCore->rendering().rendering3d().sendMesh(
            _meshRef.get(), _material, getTransform().getWorldMatrix(), _attachedToObject->instanceID());
    }
}

void
cMesh::toJson(nlohmann::json &j_out)
{
    j_out["meshPath"] = _meshPath;
    j_out["materialPath"] = _materialPath;
}

void
cMesh::fromJson(const nlohmann::json &j_in)
{
    _meshPath = j_in["meshPath"];
    _materialPath = j_in["materialPath"];

    if (!_meshPath.empty()) {
        _mesh = gCore->resources().loadResourceFromFile<jleMesh>(jleRelativePath{_meshPath});
    }
    if (!_materialPath.empty()) {
        _material = gCore->resources().loadResourceFromFile<jleMaterial>(jleRelativePath{_materialPath});
    }
}

void
cMesh::editorUpdate(float dt)
{
    update(dt);
}
