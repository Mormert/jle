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

#include "jleNetworkEvent.h"
#include "jleSceneNetworked.h"

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

    void sendNetworkEventToUser(std::unique_ptr<jleServerToClientEvent> event, int32_t userId);

    template <class Archive>
    void serialize(Archive &archive);

protected:
    void setupObject(const std::shared_ptr<jleObject> &obj) override;

private:
    void updateServerSceneObjects(float dt);

    void setupObjectForNetworking(const std::shared_ptr<jleObject> &obj);

    void processNetwork() override;

    void objectDestructionNetworked(const std::shared_ptr<jleObject>& object);

    ENetHost *_server = nullptr;

    std::unordered_map<int32_t, std::vector<std::weak_ptr<jleObject>>> _playerOwnedObjects;

    std::vector<std::unique_ptr<jleServerToClientEvent>> _eventsBroadcastQueue;

    std::unordered_map<int32_t, std::vector<std::unique_ptr<jleServerToClientEvent>>> _eventsSpecificUserQueue;

    int32_t _entityIdGenerateCounter{1};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleSceneServer)

CEREAL_REGISTER_TYPE(jleSceneServer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedOnlyResource, jleSceneServer)
