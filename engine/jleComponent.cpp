// Copyright (c) 2022. Johan Lind

#include "jleComponent.h"

#include "jleObject.h"

jleComponent::jleComponent(jleObject *owner, jleScene *scene)
    : _attachedToObject{owner}, _containedInScene{scene} {}

void jleComponent::Destroy() { _attachedToObject->DestroyComponent(this); }

void to_json(nlohmann::json& j, const std::shared_ptr<jleComponent> c) {
    c->ToJson(j);
    j += {"_comp_name", c->GetComponentName()};
}

void from_json(const nlohmann::json& j, std::shared_ptr<jleComponent>& c) {}
