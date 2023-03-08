// Copyright (c) 2023. Johan Lind

#include "jleComponent.h"
#include "jleObject.h"

template <typename T>
std::shared_ptr<T>
jleComponent::addDependencyComponentInStart()
{
    return _attachedToObject->addDependencyComponent<T>(this);
}