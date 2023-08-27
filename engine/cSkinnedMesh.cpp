// Copyright (c) 2023. Johan Lind

#include "cSkinnedMesh.h"
#include "jleAnimationFinalMatrices.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cSkinnedMesh)

cSkinnedMesh::cSkinnedMesh(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void
cSkinnedMesh::editorUpdate(float dt)
{
    static std::shared_ptr<jleAnimationFinalMatrices> identityMatrices = std::make_shared<jleAnimationFinalMatrices>();

    if (_skinnedMeshRef) {
        std::shared_ptr<jleSkinnedMesh> mesh = _skinnedMeshRef.get();
        std::shared_ptr<jleMaterial> material = _materialRef.get();
        gEngine->renderGraph().sendSkinnedMesh(
            mesh, material, identityMatrices, getTransform().getWorldMatrix(), _attachedToObject->instanceID(), true);
    }
}

void
cSkinnedMesh::start()
{

}

void
cSkinnedMesh::update(float dt)
{

}

std::shared_ptr<jleSkinnedMesh>
cSkinnedMesh::getMesh()
{
    return _skinnedMeshRef.get();
}

std::shared_ptr<jleMaterial>
cSkinnedMesh::getMaterial()
{
    return _materialRef.get();
}

jleResourceRef<jleSkinnedMesh> &
cSkinnedMesh::getMeshRef()
{
    return _skinnedMeshRef;
}

jleResourceRef<jleMaterial> &
cSkinnedMesh::getMaterialRef()
{
    return _materialRef;
}

template <class Archive>
void
cSkinnedMesh::serialize(Archive &ar)
{
    ar(CEREAL_NVP(_skinnedMeshRef), CEREAL_NVP(_materialRef));
}
