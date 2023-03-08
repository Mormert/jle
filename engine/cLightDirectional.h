// Copyright (c) 2023. Johan Lind

#pragma once

#include "cLight.h"

class cLightDirectional : public cLight
{
    JLE_REGISTER_COMPONENT_TYPE(cLightDirectional)
public:
    explicit cLightDirectional(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(cereal::base_class<cLight>(this));
    }

    void update(float dt) override;
};

CEREAL_REGISTER_TYPE(cLightDirectional)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cLight, cLightDirectional)
