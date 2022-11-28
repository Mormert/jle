// Copyright (c) 2022. Johan Lind

#pragma once

#include "cLight.h"

class cLightDirectional : public cLight
{
    JLE_REGISTER_COMPONENT_TYPE(cLightDirectional)
public:
    explicit cLightDirectional(jleObject *owner = nullptr, jleScene *scene = nullptr);

    void update(float dt) override;
};