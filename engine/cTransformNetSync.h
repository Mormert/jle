// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleComponent.h"

class cTransformNetSync: public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cTransformNetSync)
public:
    explicit cTransformNetSync(jleObject *owner = nullptr, jleScene *scene = nullptr);

    void netSyncOut(cereal::BinaryOutputArchive &ar) override;

    void netSyncIn(cereal::BinaryInputArchive &ar) override;

    void serverUpdate(float dt) override;

protected:
    glm::vec3 _color{1.f};
};

CEREAL_REGISTER_TYPE(cTransformNetSync)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cTransformNetSync)

