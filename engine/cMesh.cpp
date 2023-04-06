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
        std::shared_ptr<jleMesh> mesh = _meshRef.get();
        gCore->rendering().rendering3d().sendMesh(
            mesh, _material, getTransform().getWorldMatrix(), _attachedToObject->instanceID());
    }
}

void
cMesh::editorUpdate(float dt)
{
    update(dt);
}
