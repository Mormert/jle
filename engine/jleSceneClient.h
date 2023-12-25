// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleSceneNetworked.h"
#include "jleNetworkEvent.h"

class jleSceneClient : public jleSceneNetworked
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleSceneClient, "scn")

    ~jleSceneClient() override;

    void connectToServer(int port = 314, const char *ipAddress = "127.0.0.1");

    void disconnectFromServer();

    void onSceneStart() override;

    void onSceneDestruction() override;

    void updateScene(float dt) override;

    void sceneInspectorImGuiRender() override;

    void sendNetworkEvent(std::unique_ptr<jleNetworkEvent> event);

    template <class Archive>
    void serialize(Archive &archive);

private:
    void processNetwork() override;

    static jleSceneClient &getSceneClientRef(librg_world *w);

    static int32_t clientReadCreate(librg_world *w, librg_event *e);
    static int32_t clientReadRemove(librg_world *w, librg_event *e);
    static int32_t clientReadUpdate(librg_world *w, librg_event *e);

    ENetHost *_client = nullptr;
    ENetPeer *_peer = nullptr;

    std::vector<std::unique_ptr<jleNetworkEvent>> _eventsQueue;

    //std::unordered_map<uint64_t, std::weak_ptr<jleObject>> _networkedObjects;
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleSceneClient)

CEREAL_REGISTER_TYPE(jleSceneClient)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedOnlyResource, jleSceneClient)