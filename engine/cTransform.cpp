// Copyright (c) 2022. Johan Lind

#include "cTransform.h"

#include "jleJson.h"

namespace glm {
using namespace nlohmann;

void to_json(json &j, const vec4 &v) {
    j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w}};
}

void from_json(const json &j, vec4 &v) {
    v.x = j["x"];
    v.y = j["y"];
    v.z = j["z"];
    v.w = j["w"];
}

void to_json(json &j, const mat4 &m) {
    j = json{{"v0", m[0]}, {"v1", m[1]}, {"v2", m[2]}, {"v3", m[3]}};
}

void from_json(const json &j, mat4 &m) {
    m[0] = j["v0"];
    m[1] = j["v1"];
    m[2] = j["v2"];
    m[3] = j["v3"];
}

} // namespace glm

cTransform::cTransform(jleObject *owner, jleScene *scene)
    : jleComponent{owner, scene} {}

void
cTransform::toJson(nlohmann::json &j_out)
{
    j_out = nlohmann::json{{"transform", _local}};
}

void
cTransform::fromJson(const nlohmann::json &j_in)
{
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _local, "transform", glm::mat4{1.f});
    propagateMatrix();
}

void
cTransform::start()
{
    propagateMatrix();
}
