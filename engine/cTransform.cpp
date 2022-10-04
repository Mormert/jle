// Copyright (c) 2022. Johan Lind

#include "cTransform.h"

#include "jleJson.h"

cTransform::cTransform(jleObject *owner, jleScene *scene)
    : jleComponent{owner, scene} {}

void cTransform::toJson(nlohmann::json& j_out) {
    j_out = nlohmann::json{{"x", _x}, {"y", _y}, {"depth", _depth}};
}

void cTransform::fromJson(const nlohmann::json& j_in) {
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _x, "x", 0.f);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _y, "y", 0.f);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _depth, "depth", 0.f);
}