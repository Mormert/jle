/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#pragma once

#include "core/jleCommon.h"

#include "jleSceneNetworked.h"
#include "modules/networking/jleNetworkEventOutQueue.h"

constexpr int serverOwnedId = 0;

class jleSceneServer : public jleSceneNetworked
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleSceneServer, "scn")

    ~jleSceneServer() override;

    int startServer(jleEngineModulesContext& ctx, int port = 314, int maxClients = 4);

    int stopServer();

    void onSceneStart(jleEngineModulesContext& ctx) override;

    void onSceneDestruction() override;

    void updateScene(jleEngineModulesContext& ctx) override;

    void sceneInspectorImGuiRender() override;

    std::shared_ptr<jleObject> spawnObjectWithOwner(jleEngineModulesContext& ctx, const std::string &objectName, int32_t ownerId);

    void sendNetworkEventBroadcast(std::unique_ptr<jleServerToClientEvent> event);

    void sendNetworkEventToUser(std::unique_ptr<jleServerToClientEvent> event, int32_t userId);

    template <class Archive>
    void serialize(Archive &archive);

    std::shared_ptr<jleObject> getObjectFromNetId(int32_t netId);

protected:
    void setupObject(const std::shared_ptr<jleObject> &obj, jleSerializationContext& ctx) override;

    virtual void
    onClientConnect(jleEngineModulesContext& ctx, int32_t clientId)
    {
    }

    virtual void
    onClientDisconnect(int32_t clientId)
    {
    }

private:
    void updateServerSceneObjects(jleEngineModulesContext& ctx);

    void setupObjectForNetworking(const std::shared_ptr<jleObject> &obj);

    void processNetwork(jleEngineModulesContext& ctx) override;

    void objectDestructionNetworked(const std::shared_ptr<jleObject> &object);

    void destroyAllClientOwnedObjects(int32_t clientId);

    void setNetIdObject(const std::shared_ptr<jleObject> &object, int32_t netId);

    ENetHost *_server = nullptr;

    std::unordered_map<int32_t, std::vector<std::weak_ptr<jleObject>>> _playerOwnedObjects;

    jleNetworkEventOutQueue _eventsBroadcastQueue;

    std::unordered_map<int32_t, jleNetworkEventOutQueue> _eventsSpecificUserQueue;

    std::unordered_map<int32_t, std::weak_ptr<jleObject>> _networkedObjects;

    int32_t _entityIdGenerateCounter{1};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleSceneServer)

CEREAL_REGISTER_TYPE(jleSceneServer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedOnlyResource, jleSceneServer)
