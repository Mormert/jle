// Copyright (c) 2023. Johan Lind

#include "cLight.h"

#include "jleCore.h"
#include "jleRendering.h"

namespace glm
{
using namespace nlohmann;

void
to_json(json &j, const vec3 &v)
{
    j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}};
}

void
from_json(const json &j, vec3 &v)
{
    v.x = j["x"];
    v.y = j["y"];
    v.z = j["z"];
}
} // namespace glm

cLight::cLight(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}
void
cLight::start()
{
    //_transform = addDependencyComponentInStart<cTransform>();
}
void
cLight::update(float dt)
{
    gCore->rendering().rendering3d().sendLight(getTransform().getWorldPosition(), _color);
}
void
cLight::toJson(nlohmann::json &j_out)
{
    j_out["color"] = _color;
}
void
cLight::fromJson(const nlohmann::json &j_in)
{
    _color = j_in["color"];
}
