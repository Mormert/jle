// Copyright (c) 2022. Johan Lind

#include "jleComponent.h"

#include "jleObject.h"

jleComponent::jleComponent(jleObject *owner, jleScene *scene) : _attachedToObject{owner}, _containedInScene{scene} {}

void
jleComponent::destroy()
{
    _attachedToObject->destroyComponent(this);
}

void
to_json(nlohmann::json &j, const std::shared_ptr<jleComponent> c)
{
    c->toJson(j);
    j += {"_comp_name", c->componentName()};
}

void
from_json(const nlohmann::json &j, std::shared_ptr<jleComponent> &c)
{
}

jleTransform &
jleComponent::getTransform()
{
    return _attachedToObject->getTransform();
}
