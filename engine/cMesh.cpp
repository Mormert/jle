// Copyright (c) 2023. Johan Lind

#include "cMesh.h"
#include "jleCore.h"
#include "jleResource.h"
#include "jleRendering.h"

cMesh::cMesh(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void
cMesh::start()
{}

void
cMesh::update(float dt)
{
    if (_meshRef) {
        std::shared_ptr<jleMesh> mesh = _meshRef.get();
        std::shared_ptr<jleMaterial> material = _materialRef.get();
        gCore->rendering().rendering3d().sendMesh(
            mesh, material, getTransform().getWorldMatrix(), _attachedToObject->instanceID());
    }
}

void
cMesh::editorUpdate(float dt)
{
    update(dt);
}
