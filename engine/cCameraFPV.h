// Copyright (c) 2022. Johan Lind

#pragma once

#include "cCamera.h"
#include "jleCamera.h"

class cCameraFPV : public cCamera
{
    JLE_REGISTER_COMPONENT_TYPE(cCameraFPV)
public:
    explicit cCameraFPV(jleObject *owner = nullptr, jleScene *scene = nullptr);

    ~cCameraFPV() override;

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

    void update(float dt) override;

protected:
    jleCameraSimpleFPVController _fpvController;

    float _moveSpeed{10.f};
    float _mouseSensitivity{1.f};
};
