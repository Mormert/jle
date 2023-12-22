// Copyright (c) 2023. Johan Lind

#include "jleNetworkEvent.h"
#include "jleSceneServer.h"

template <class Archive>
void
jleNetworkEvent::serialize(Archive &archive)
{
}

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleNetworkEvent)

jleSceneServer &
jleNetworkEvent::getSceneServer()
{
    return *_serverScene;
}