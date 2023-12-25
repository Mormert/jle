// Copyright (c) 2023. Johan Lind

#include "jleNetworkEvent.h"
#include "jleSceneServer.h"


jleSceneServer &
jleClientToServerEvent::getSceneServer()
{
    return *_serverScene;
}

jleSceneClient &
jleServerToClientEvent::getSceneClient()
{
    return *_clientScene;
}
