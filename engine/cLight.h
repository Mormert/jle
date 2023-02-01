// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleComponent.h"

#include <glm/vec3.hpp>

class cLight : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cLight)
public:
    explicit cLight(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_color));
    }

    void start() override;

    void update(float dt) override;

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

protected:
    glm::vec3 _color{1.f};
};

CEREAL_REGISTER_TYPE(cLight)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cLight)
