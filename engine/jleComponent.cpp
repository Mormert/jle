// Copyright (c) 2022. Johan Lind

#include "jleComponent.h"

#include "jleObject.h"

jleComponent::jleComponent(jleObject *owner, jleScene *scene)
    : mAttachedToObject{owner}, mContainedInScene{scene} {}

void jleComponent::Destroy() { mAttachedToObject->DestroyComponent(this); }

void to_json(nlohmann::json& j, const std::shared_ptr<jleComponent> c) {
    c->ToJson(j);
    j += {"_comp_name", c->GetComponentName()};
}

void from_json(const nlohmann::json& j, std::shared_ptr<jleComponent>& c) {}
