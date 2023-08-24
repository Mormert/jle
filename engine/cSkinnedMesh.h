// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleComponent.h"

#include "jleSkinnedMesh.h"
#include "jleMaterial.h"

class cSkinnedMesh : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cSkinnedMesh)
public:
    explicit cSkinnedMesh(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void serialize(Archive &ar);

    void editorUpdate(float dt) override;

    void start() override;

    void update(float dt) override;

    std::shared_ptr<jleSkinnedMesh> getMesh();
    std::shared_ptr<jleMaterial> getMaterial();

    jleResourceRef<jleSkinnedMesh>& getMeshRef();
    jleResourceRef<jleMaterial>& getMaterialRef();

protected:

    jleResourceRef<jleSkinnedMesh> _skinnedMeshRef;
    jleResourceRef<jleMaterial> _materialRef;

};

JLE_EXTERN_TEMPLATE_CEREAL_H(cSkinnedMesh)

CEREAL_REGISTER_TYPE(cSkinnedMesh)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cSkinnedMesh)