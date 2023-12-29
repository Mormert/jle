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
jleSceneClient::updateScene(float dt)
{
    jleScene::updateScene(dt);
    processNetwork();
}

void
jleSceneClient::onSceneStart()
{
    connectToServer();
}

void
jleSceneClient::onSceneDestruction()
{
    disconnectFromServer();
}
void
jleSceneClient::processNetwork()
{
    JLE_SCOPE_PROFILE_CPU(jleSceneClientProcessNetwork)

    ENetEvent event{};

    const auto localClientSceneId = _client->peers[0].outgoingPeerID + 1;

    while (enet_host_service(_client, &event, 0) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT: {
            LOGI << "[client] A client scene has connected to the server";
        } break;
        case ENET_EVENT_TYPE_DISCONNECT:
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
            LOGI << "[client] A client scene has disconnected from the server";
        } break;
        case ENET_EVENT_TYPE_RECEIVE: {
            // Retrieve the op code as the first byte
            auto opCode = static_cast<jleNetOpCode>(event.packet->data[0]);

            // Packet actual data comes after op code byte
            auto *dataBuffer = &event.packet->data[1];
            const auto dataLength = event.packet->dataLength - 1;

            switch (opCode) {
            case jleNetOpCode::Events: {

                std::vector<std::unique_ptr<jleServerToClientEvent>> events;

                std::string bufferAsString((char *)dataBuffer, dataLength);

                std::stringstream stream{};
                stream << bufferAsString;

                try {
                    cereal::BinaryInputArchive archive(stream);
                    archive(events);

                    for (auto &e : events) {
                        e->_clientScene = this;
                        e->execute();
                    }
                } catch (std::exception &e) {
                    LOGE << "[server] failed to parse event data: " << e.what();
                }

            } break;

            case jleNetOpCode::WorldWrite: {

            } break;
            }

            enet_packet_destroy(event.packet);
        } break;
        case ENET_EVENT_TYPE_NONE:
            break;
        }
    }

    if (!_eventsQueue.empty()) {
        std::ostringstream oss;

        // Insert the first byte as the op code
        oss << static_cast<char>(jleNetOpCode::Events);

        try { // Note that we need the archive to go out of scope to completely fill the string stream!
            cereal::BinaryOutputArchive archive(oss);
            archive(_eventsQueue);
        } catch (std::exception &e) {
            LOGE << "Failed to write event data: " << e.what();
        }

        auto writeBufferString = oss.str();
        auto *packetBuffer = writeBufferString.data();
        const size_t packetBufferLen = writeBufferString.size();

        ENetPacket *packet = enet_packet_create(packetBuffer, packetBufferLen, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(_peer, 0, packet);

        _eventsQueue.clear();
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
    _eventsQueue.push_back(std::move(event));
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
jleSceneClient::spawnObjectFromServer(const std::shared_ptr<jleObject> &object, int32_t netId, int32_t owner)
{
    setNetIdObject(object, netId);
    object->_netId = netId;
    object->_networkOwnerID = owner;
    object->_isStarted = true;
    setupObjectForNetworking(object);
    _sceneObjects.push_back(object);
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
}
