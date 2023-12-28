// Copyright (c) 2023. Johan Lind

#include "cTransformNetSync.h"

cTransformNetSync::cTransformNetSync(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void
cTransformNetSync::netSyncOut(cereal::BinaryOutputArchive &ar)
{
    auto &m = object()->getTransform().getLocalMatrix();
    ar(m);
}

void
cTransformNetSync::netSyncIn(cereal::BinaryInputArchive &ar)
{
    glm::mat4 m;
    ar(m);
    object()->getTransform().setLocalMatrix(m);
}

void
cTransformNetSync::serverUpdate(float dt)
{
    syncServerToClient();
}
