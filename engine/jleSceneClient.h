// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleNetworkEvent.h"
#include "jleSceneNetworked.h"

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

    void spawnObjectFromServer(const std::shared_ptr<jleObject> &object, int32_t netId, int32_t owner = 0);

    void sendNetworkEvent(std::unique_ptr<jleClientToServerEvent> event);

    std::shared_ptr<jleObject> getObjectFromNetId(int32_t netId);

    void setNetIdObject(const std::shared_ptr<jleObject> &object, int32_t netId);

    template <class Archive>
    void serialize(Archive &archive);

protected:
    void setupObject(const std::shared_ptr<jleObject> &obj) override;

private:
    void processNetwork() override;

    void setupObjectForNetworking(const std::shared_ptr<jleObject> &obj);

    ENetHost *_client = nullptr;
    ENetPeer *_peer = nullptr;

    std::vector<std::unique_ptr<jleClientToServerEvent>> _eventsQueue;

    std::unordered_map<int32_t, std::weak_ptr<jleObject>> _networkedObjects;
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleSceneClient)

CEREAL_REGISTER_TYPE(jleSceneClient)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedOnlyResource, jleSceneClient)