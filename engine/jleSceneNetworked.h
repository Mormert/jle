// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleScene.h"

typedef struct _ENetHost ENetHost;
typedef struct _ENetPeer ENetPeer;

enum class jleNetOpCode : int8_t { Events, WorldWrite };

class jleSceneNetworked : public jleScene
{
protected:
    virtual void processNetwork() = 0;

    static void networkSceneDisplayInspectorWindow(const std::string &sceneType,
                                                   const std::string &sceneName,
                                                   ENetHost *host);
};