// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleComponent.h"
#include "jleMaterial.h"
#include "jleMesh.h"
#include "jleTransform.h"

#include "jleResourceRef.h"
#include "editor/jleImGuiCerealArchive.h"

class cRigidbody : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cRigidbody)
public:
    explicit cRigidbody(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_mass), CEREAL_NVP(_origin));
    }

    void editorUpdate(float dt) override;

    void start() override;

    void update(float dt) override;

protected:

    JLE_TOOLTIP_ARITHMETIC(float, "Setting mass to 0 makes this rigidbody static", _mass);

    glm::vec3 _origin;
};

CEREAL_REGISTER_TYPE(cRigidbody)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cRigidbody)
