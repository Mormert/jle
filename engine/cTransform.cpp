// Copyright (c) 2022. Johan Lind

#include "cTransform.h"

#include "jleJson.h"

cTransform::cTransform(jleObject *owner, jleScene *scene)
    : jleComponent{owner, scene} {}

void cTransform::ToJson(nlohmann::json& j_out) {
    j_out = nlohmann::json{{"x", mX}, {"y", mY}, {"depth", mDepth}};
}

void cTransform::FromJson(const nlohmann::json& j_in) {
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mX, "x", 0.f);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mY, "y", 0.f);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mDepth, "depth", 0.f);
}