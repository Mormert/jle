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

#include "jleSceneClient.h"
#include "jleNetworkEvent.h"
#include "jleProfiler.h"

#include <enet.h>

void
jleSceneClient::connectToServer(int port, const char *ipAddress)
{
    ENetAddress address = {0};
    address.port = port;
    enet_address_set_host(&address, ipAddress);

    _client = enet_host_create(nullptr, 1, 2, 0, 0);
    _peer = enet_host_connect(_client, &address, 2, 0);

    ENetCompressor compressor;
    initializeENetCompressor(compressor);
    enet_host_compress(_client, &compressor);

    if (_peer == nullptr) {
        LOGE << "[client] Cannot connect";
        return;
    }

    // Disable timeouts for the peer
    enet_peer_timeout(_peer, 0, 0, 0);
}

void
jleSceneClient::disconnectFromServer()
{
    if (_peer) {
        enet_peer_disconnect_now(_peer, 0);
        _peer = nullptr;
    }
    if (_client) {
        enet_host_destroy(_client);
        _client = nullptr;
    }

    LOGI << "[client] Disconnected scene client: " << path.getVirtualPath();
}

jleSceneClient::~jleSceneClient() { disconnectFromServer(); }

void
jleSceneClient::updateScene(jleEngineModulesContext &ctx)
{
    jleScene::updateScene(ctx);
    processNetwork(ctx);
}

void
jleSceneClient::onSceneStart()
{
    connectToServer(_portToConnectTo, _ipAddressToConnectTo.c_str());
}

void
jleSceneClient::onSceneDestruction()
{
    disconnectFromServer();
}
void
jleSceneClient::processNetwork(jleEngineModulesContext& ctx)
{
    JLE_SCOPE_PROFILE_CPU(jleSceneClientProcessNetwork)

    ENetEvent event{};

    while (enet_host_service(_client, &event, 0) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT: {
            LOGI << "[client] A client scene has connected to the server";
            _clientId = _client->peers[0].outgoingPeerID + 1;
        } break;
        case ENET_EVENT_TYPE_DISCONNECT:
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
            LOGI << "[client] A client scene has disconnected from the server";
        } break;
        case ENET_EVENT_TYPE_RECEIVE: {
            const char *dataBuffer = reinterpret_cast<char *>(event.packet->data);
            const auto dataLength = event.packet->dataLength;

            jleExecuteServerEventsOnClient(ctx, dataBuffer, dataLength, this);

            enet_packet_destroy(event.packet);
        } break;
        case ENET_EVENT_TYPE_NONE:
            break;
        }
    }

    if (!_eventsQueueToServer.isEmpty()) {
        const auto data = _eventsQueueToServer.data();

        auto *packetBuffer = &data[0];
        const size_t packetBufferLen = data.size();

        ENetPacket *packet = enet_packet_create(packetBuffer, packetBufferLen, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(_peer, 0, packet);

        _eventsQueueToServer.resetQueue();
    }
}

void
jleSceneClient::sceneInspectorImGuiRender()
{
    networkSceneDisplayInspectorWindow("Client", sceneName, _client);
}

void
jleSceneClient::sendNetworkEvent(std::unique_ptr<jleClientToServerEvent> event)
{
    _eventsQueueToServer.enqueue(std::move(event));
}

std::shared_ptr<jleObject>
jleSceneClient::getObjectFromNetId(int32_t netId)
{
    auto it = _networkedObjects.find(netId);
    if (it == _networkedObjects.end()) {
        return nullptr;
    }
    if (it->second.expired()) {
        _networkedObjects.erase(it);
        return nullptr;
    }
    return it->second.lock();
}

void
jleSceneClient::setNetIdObject(const std::shared_ptr<jleObject> &object, int32_t netId)
{
    _networkedObjects.insert(std::make_pair(netId, object));
}

void
jleSceneClient::spawnObjectFromServer(jleEngineModulesContext &ctx,
                                      const std::shared_ptr<jleObject> &object,
                                      int32_t netId,
                                      int32_t owner)
{
    setNetIdObject(object, netId);
    object->_netId = netId;
    object->_networkOwnerID = owner;
    setupObjectForNetworking(object);
    _sceneObjects.push_back(object);

    object->startComponents(ctx);
    object->_isStarted = true;
}

int32_t
jleSceneClient::clientId() const
{
    return _clientId;
}

void
jleSceneClient::setupObject(const std::shared_ptr<jleObject> &obj)
{
    jleScene::setupObject(obj);
    setupObjectForNetworking(obj);
}

void
jleSceneClient::setupObjectForNetworking(const std::shared_ptr<jleObject> &obj)
{
    obj->propagateOwnedBySceneClient(this);
}

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleSceneClient)

template <class Archive>
void
jleSceneClient::serialize(Archive &archive)
{
    archive(cereal::base_class<jleScene>(this));

    try {
        archive(CEREAL_NVP(_ipAddressToConnectTo), CEREAL_NVP(_portToConnectTo));
    } catch (std::exception &e) {
    }
}
