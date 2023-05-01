// Copyright (c) 2023. Johan Lind

#pragma once

#include "cCamera.h"
#include "jleCamera.h"

class cCameraFPV : public cCamera
{
    JLE_REGISTER_COMPONENT_TYPE(cCameraFPV)
public:
    explicit cCameraFPV(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(cereal::base_class<cCamera>(this));
        ar(CEREAL_NVP(_moveSpeed), CEREAL_NVP(_mouseSensitivity));
    }

    ~cCameraFPV() override;

    void update(float dt) override;

protected:
    jleCameraSimpleFPVController _fpvController;

    float _moveSpeed{10.f};
    float _mouseSensitivity{1.f};
};

CEREAL_REGISTER_TYPE(cCameraFPV)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cCameraFPV)
