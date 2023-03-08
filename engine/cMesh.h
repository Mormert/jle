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
        ar(CEREAL_NVP(_meshRef), CEREAL_NVP(_materialRef), CEREAL_NVP(_meshPath), CEREAL_NVP(_materialPath));
    }

    void start() override;

    void update(float dt) override;

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

protected:
    std::shared_ptr<jleMesh> _mesh;
    std::shared_ptr<jleMaterial> _material;

    jleResourceRef<jleMesh> _meshRef;
    jleResourceRef<jleMaterial> _materialRef;

    std::string _meshPath;
    std::string _materialPath;

    //std::weak_ptr<cTransform> _transform;
};

CEREAL_REGISTER_TYPE(cMesh)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cMesh)
