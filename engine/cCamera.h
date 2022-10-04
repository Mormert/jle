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

    void start() override;

    void toJson(nlohmann::json& j_out) override;

    void fromJson(const nlohmann::json& j_in) override;

    void update(float dt) override;

    float offsetX();

    float offsetY();

private:
    std::shared_ptr<cTransform> _transform{nullptr};
    float _offsetX{};
    float _offsetY{};

    inline static uint32_t sInstanceCounter = 0;
};
