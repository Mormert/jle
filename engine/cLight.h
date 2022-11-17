// Copyright (c) 2022. Johan Lind

#pragma once

#include "cTransform.h"
#include "jleComponent.h"

#include <glm/vec3.hpp>

class cLight : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cLight)
public:
    explicit cLight(jleObject *owner = nullptr, jleScene *scene = nullptr);

    void start() override;

    void update(float dt) override;

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

protected:
    std::shared_ptr<cTransform> _transform{};
    glm::vec3 _color{};
};