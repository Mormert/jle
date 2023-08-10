// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleComponent.h"
#include "jleMaterial.h"
#include "jleMesh.h"
#include "jleTransform.h"

#include "jleResourceRef.h"

class cMesh : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cMesh)
public:
    explicit cMesh(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_meshRef), CEREAL_NVP(_materialRef));
    }

    void editorUpdate(float dt) override;

    void start() override;

    void update(float dt) override;

    std::shared_ptr<jleMesh> getMesh();
    std::shared_ptr<jleMaterial> getMaterial();

protected:

    jleResourceRef<jleMesh> _meshRef;
    jleResourceRef<jleMaterial> _materialRef;

};

JLE_EXTERN_TEMPLATE_CEREAL_H(cMesh)

CEREAL_REGISTER_TYPE(cMesh)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cMesh)
