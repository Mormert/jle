// Copyright (c) 2022. Johan Lind

#pragma once

#include "cTransform.h"
#include "jleComponent.h"
#include "jleMesh.h"
#include "jleMaterial.h"

class cMesh : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cMesh)
public:
    explicit cMesh(jleObject *owner = nullptr, jleScene *scene = nullptr);

    void start() override;

    void update(float dt) override;

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

protected:
    std::shared_ptr<jleMesh> _mesh;
    std::shared_ptr<jleMaterial> _material;

    std::string _meshPath;
    std::string _materialPath;

    std::weak_ptr<cTransform> _transform;
};
