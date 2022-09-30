// Copyright (c) 2022. Johan Lind

#pragma once

#include "cTransform.h"
#include "jleAseprite.h"
#include "jleComponent.h"

class cCamera : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cCamera)
public:
    explicit cCamera(jleObject *owner = nullptr, jleScene *scene = nullptr);

    ~cCamera() override;

    void Start() override;

    void ToJson(nlohmann::json& j_out) override;

    void FromJson(const nlohmann::json& j_in) override;

    void Update(float dt) override;

    float GetOffsetX();

    float GetOffsetY();

private:
    std::shared_ptr<cTransform> mTransform{nullptr};
    float mOffsetX{};
    float mOffsetY{};

    inline static uint32_t sInstanceCounter = 0;
};
