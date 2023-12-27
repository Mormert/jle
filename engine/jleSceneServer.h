// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleSceneNetworked.h"
#include "jleNetworkEvent.h"


constexpr int serverOwnedId = 0;

class jleSceneServer : public jleSceneNetworked
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleSceneServer, "scn")

    ~jleSceneServer() override;

    int startServer(int port = 314, int maxClients = 4);

    int stopServer();

    void onSceneStart() override;

    void onSceneDestruction() override;

    void updateScene(float dt) override;

    void sceneInspectorImGuiRender() override;

    void sendNetworkEventBroadcast(std::unique_ptr<jleServerToClientEvent> event);

    template <class Archive>
    void serialize(Archive &archive);

protected:
    void setupObject(const std::shared_ptr<jleObject> &obj) override;

private:

    void updateServerSceneObjects(float dt);

    void setupObjectForNetworking(const std::shared_ptr<jleObject> &obj);

    void processNetwork() override;

    static jleSceneServer &getSceneServerRef(librg_world *w);

    static int32_t serverWriteUpdate(librg_world *w, librg_event *e);
    static int32_t serverWriteCreate(librg_world *w, librg_event *e);

    ENetHost *_server = nullptr;

    std::unordered_map<uint64_t, std::vector<std::weak_ptr<jleObject>>> _playerOwnedObjects;

    std::vector<std::unique_ptr<jleServerToClientEvent>> _eventsBroadcastQueue;

    int64_t _entityIdGenerateCounter{1};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleSceneServer)

CEREAL_REGISTER_TYPE(jleSceneServer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedOnlyResource, jleSceneServer)
