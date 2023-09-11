// Copyright (c) 2023. Johan Lind

#include "jleComponent.h"

#include "jleObject.h"

jleComponent::jleComponent(jleObject *owner, jleScene *scene) : _attachedToObject{owner}, _containedInScene{scene} {}

void
jleComponent::destroy()
{
    _attachedToObject->destroyComponent(this);
}

jleTransform &
jleComponent::getTransform()
{
    return _attachedToObject->getTransform();
}

jleObject *
jleComponent::object()
{
    return _attachedToObject;
}

jleScene *
jleComponent::scene()
{
    return _containedInScene;
}

bool
jleComponent::isDestroyed()
{
    return _isDestroyed;
}

int
jleComponent::parallelUpdateBatchSize()
{
    return _parallelUpdateBatchSize;
}
