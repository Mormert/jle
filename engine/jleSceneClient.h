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

#include "jleNetworkEventOutQueue.h"
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

    void updateScene(jleEngineModulesContext &ctx) override;

    void sceneInspectorImGuiRender() override;

    void spawnObjectFromServer(jleEngineModulesContext &ctx,
                               const std::shared_ptr<jleObject> &object,
                               int32_t netId,
                               int32_t owner = 0);

    void sendNetworkEvent(std::unique_ptr<jleClientToServerEvent> event);

    std::shared_ptr<jleObject> getObjectFromNetId(int32_t netId);

    void setNetIdObject(const std::shared_ptr<jleObject> &object, int32_t netId);

    template <class Archive>
    void serialize(Archive &archive);

    int32_t clientId() const;

protected:
    void setupObject(const std::shared_ptr<jleObject> &obj) override;

private:
    void processNetwork(jleEngineModulesContext& ctx) override;

    void setupObjectForNetworking(const std::shared_ptr<jleObject> &obj);

    ENetHost *_client = nullptr;
    ENetPeer *_peer = nullptr;

    int32_t _clientId{-1};

    jleNetworkEventOutQueue _eventsQueueToServer;

    std::unordered_map<int32_t, std::weak_ptr<jleObject>> _networkedObjects;

    std::string _ipAddressToConnectTo{"127.0.0.1"};
    int _portToConnectTo{314};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleSceneClient)

CEREAL_REGISTER_TYPE(jleSceneClient)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedOnlyResource, jleSceneClient)