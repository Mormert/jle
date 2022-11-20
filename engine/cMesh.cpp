// Copyright (c) 2022. Johan Lind

#include "cMesh.h"
#include "jleCore.h"
#include "jleResource.h"

cMesh::cMesh(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void
cMesh::start()
{
    _transform = addDependencyComponentInStart<cTransform>();
}

void
cMesh::update(float dt)
{
    if (_mesh) {
        gCore->rendering().rendering3d().sendMesh(
            _mesh, _transform.lock()->getWorldMatrix(), _attachedToObject->instanceID());
    }
}

void
cMesh::toJson(nlohmann::json &j_out)
{
    j_out["meshPath"] = _meshPath;
}

void
cMesh::fromJson(const nlohmann::json &j_in)
{
    _meshPath = j_in["meshPath"];
    if (!_meshPath.empty()) {
        _mesh = gCore->resources().loadResourceFromFile<jleMesh>(jleRelativePath{_meshPath});
    }
}
