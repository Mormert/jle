#include "jleComponent.h"

#include "jleObject.h"

jle::jleComponent::jleComponent(jleObject* owner, jleScene* scene) : mAttachedToObject {owner}, mContainedInScene {scene} {}
