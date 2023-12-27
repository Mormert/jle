// Copyright (c) 2023. Johan Lind

#include "jleSceneServer.h"
#include "jleEngineSettings.h"
#include "jleNetworkEvent.h"
#include "jleProfiler.h"
#include "jleSceneClient.h"

#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>

#include <enet.h>

struct jleCreateObjectEvent : public jleServerToClientEvent {
    void
    execute() override
    {
        auto &scene = getSceneClient();
        if (newObject) {
            scene.spawnObjectFromServer(newObject, objectNetId, objectOwnerId);
        }
    }

    template <class Archive>
    void
    serialize(Archive &archive)
    {
        archive(CEREAL_NVP(objectNetId), CEREAL_NVP(newObject), CEREAL_NVP(objectOwnerId));
    }

    int32_t objectNetId{};
    int32_t objectOwnerId{};
    std::shared_ptr<jleObject> newObject;
};

JLE_REGISTER_NET_EVENT(jleCreateObjectEvent)

struct jleDestroyObjectEvent : public jleServerToClientEvent {
    void
    execute() override
    {
        auto &scene = getSceneClient();
        if (auto object = scene.getObjectFromNetId(objectNetId)) {
            object->destroyObject();
        }
    }

    template <class Archive>
    void
    serialize(Archive &archive)
    {
        archive(CEREAL_NVP(objectNetId));
    }

    int32_t objectNetId{};
};

JLE_REGISTER_NET_EVENT(jleDestroyObjectEvent)

struct jleFullSceneSyncEvent : public jleServerToClientEvent {
    void
    execute() override
    {
        auto &scene = getSceneClient();
        for (int i = 0; i < objects.size(); i++) {
            scene.spawnObjectFromServer(objects[i], objectsNetId[i]);
        }
    }

    template <class Archive>
    void
    serialize(Archive &archive)
    {
        archive(CEREAL_NVP(objects), CEREAL_NVP(objectsNetId));
    }

    std::vector<std::shared_ptr<jleObject>> objects;
    std::vector<int32_t> objectsNetId;
};

JLE_REGISTER_NET_EVENT(jleFullSceneSyncEvent)

int
jleSceneServer::startServer(int port, int maxClients)
{
    if (!gEngine->settings().enableNetworking) {
        LOGE << "[server] Trying to use networked features but networking disabled in settings";
        return 1;
    }

    ENetAddress address = {0};

    address.host = ENET_HOST_ANY;
    address.port = port;

    _server = enet_host_create(&address, maxClients, 2, 0, 0);

    if (_server == nullptr) {
        LOGE << "[server] An error occurred while trying to create an ENet server host.";
        return 1;
    }

    LOGI << "[server] Started an ENet server...";

    for (auto &object : _sceneObjects) {
        setupObjectForNetworking(object);
    }

    spawnObjectWithName("server_dummy");

    return 0;
}

int
jleSceneServer::stopServer()
{
    enet_host_destroy(_server);

    LOGI << "[server] Stopped scene server: " << path.getVirtualPath();

    return 0;
}

jleSceneServer::~jleSceneServer() { stopServer(); }

void
jleSceneServer::updateScene(float dt)
{
    processNewSceneObjects();
    updateServerSceneObjects(dt);

    processNetwork();
}

void
jleSceneServer::onSceneStart()
{
    startServer();
}

void
jleSceneServer::onSceneDestruction()
{
    stopServer();
}

void
jleSceneServer::processNetwork()
{
    JLE_SCOPE_PROFILE_CPU(jleSceneServer_processNetwork)

    {
        JLE_SCOPE_PROFILE_CPU(jleSceneServer_updateScene_EnetService)

        ENetEvent event{};

        while (enet_host_service(_server, &event, 0) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                const int32_t incomingPlayerID = event.peer->incomingPeerID + 1;
                LOGI << "[server] A new user " << incomingPlayerID << " connected.";

                // Disable timeouts for the peer
                enet_peer_timeout(event.peer, 0, 0, 0);

                auto &thisPlayersObjects = _playerOwnedObjects[incomingPlayerID] = {};

                auto player = spawnObjectWithName("player_" + std::to_string(incomingPlayerID));
                player->_networkOwnerID = incomingPlayerID;
                thisPlayersObjects.push_back(player);

                auto syncEvent = jleMakeNetEvent<jleFullSceneSyncEvent>();
                syncEvent->objects = _sceneObjects;
                for (auto &object : _sceneObjects) {
                    syncEvent->objectsNetId.push_back(object->netID());
                }
                sendNetworkEventToUser(std::move(syncEvent), incomingPlayerID);

            } break;
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                const auto incomingPlayerID = event.peer->incomingPeerID + 1;
                LOGI << "[server] A user " << incomingPlayerID << " disconnected.";

                auto &thisPlayersObjects = _playerOwnedObjects[incomingPlayerID];
                for (auto &playerObject : thisPlayersObjects) {
                    if (auto &&sharedPtr = playerObject.lock()) {
                        auto event = jleMakeNetEvent<jleDestroyObjectEvent>();
                        event->objectNetId = sharedPtr->netID();
                        sharedPtr->destroyObject();
                    }
                }
            } break;

            case ENET_EVENT_TYPE_RECEIVE: {
                const auto incomingPlayerID = event.peer->incomingPeerID + 1;

                // Retrieve the op code as the first byte
                auto opCode = static_cast<jleNetOpCode>(event.packet->data[0]);

                // Packet actual data comes after op code byte
                auto *dataBuffer = &event.packet->data[1];
                const auto dataLength = event.packet->dataLength - 1;

                switch (opCode) {
                case jleNetOpCode::Events: {
                    std::vector<std::unique_ptr<jleClientToServerEvent>> events;

                    std::string bufferAsString((char *)dataBuffer, dataLength);

                    std::stringstream stream{};
                    stream << bufferAsString;

                    try {
                        cereal::BinaryInputArchive archive(stream);
                        archive(events);

                        for (auto &e : events) {
                            e->_serverScene = this;
                            e->_clientId = incomingPlayerID;
                            e->execute();
                        }
                    } catch (std::exception &e) {
                        LOGE << "[server] failed to parse event data: " << e.what();
                    }

                } break;
                case jleNetOpCode::WorldWrite: {
                } break;
                default:
                    break;
                }

                // Clean up the packet now that we're done using it.
                enet_packet_destroy(event.packet);
            } break;

            case ENET_EVENT_TYPE_NONE:
                break;
            }
        }
    }

    if (!_eventsBroadcastQueue.empty()) {
        std::ostringstream oss;

        // Insert the first byte as the op code
        oss << static_cast<char>(jleNetOpCode::Events);

        try { // Note that we need the archive to go out of scope to completely fill the string stream!
            cereal::BinaryOutputArchive archive(oss);
            archive(_eventsBroadcastQueue);
        } catch (std::exception &e) {
            LOGE << "Failed to write event data: " << e.what();
        }

        auto writeBufferString = oss.str();
        auto *packetBuffer = writeBufferString.data();
        const size_t packetBufferLen = writeBufferString.size();

        for (ENetPeer *currentPeer = _server->peers; currentPeer < &_server->peers[_server->peerCount]; ++currentPeer) {
            if (currentPeer->state != ENET_PEER_STATE_CONNECTED) {
                continue;
            }

            ENetPacket *packet = enet_packet_create(packetBuffer, packetBufferLen, ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(currentPeer, 0, packet);
        }
        _eventsBroadcastQueue.clear();
    }

    if (!_eventsSpecificUserQueue.empty()) {
        for (auto &userEvents : _eventsSpecificUserQueue) {
            std::ostringstream oss;

            const auto peerId = userEvents.first - 1;
            const auto &eventArray = userEvents.second;

            // Insert the first byte as the op code
            oss << static_cast<char>(jleNetOpCode::Events);

            try { // Note that we need the archive to go out of scope to completely fill the string stream!
                cereal::BinaryOutputArchive archive(oss);
                archive(eventArray);
            } catch (std::exception &e) {
                LOGE << "Failed to write event data: " << e.what();
            }

            auto writeBufferString = oss.str();
            auto *packetBuffer = writeBufferString.data();
            const size_t packetBufferLen = writeBufferString.size();

            for (ENetPeer *currentPeer = _server->peers; currentPeer < &_server->peers[_server->peerCount];
                 ++currentPeer) {
                if (currentPeer->state != ENET_PEER_STATE_CONNECTED) {
                    continue;
                }
                if (currentPeer->incomingPeerID != peerId) {
                    continue;
                }

                ENetPacket *packet = enet_packet_create(packetBuffer, packetBufferLen, ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(currentPeer, 0, packet);
            }
        }

        _eventsSpecificUserQueue.clear();
    }
}

void
jleSceneServer::setupObject(const std::shared_ptr<jleObject> &obj)
{
    jleScene::setupObject(obj);
    setupObjectForNetworking(obj);
}

void
jleSceneServer::setupObjectForNetworking(const std::shared_ptr<jleObject> &obj)
{
    const auto entityId = _entityIdGenerateCounter++;
    obj->_netId = entityId;
    obj->_networkOwnerID = serverOwnedId;

    if (!_server) {
        return;
    }

    auto event = jleMakeNetEvent<jleCreateObjectEvent>();
    event->objectNetId = entityId;
    event->newObject = obj;
    event->objectOwnerId = obj->_networkOwnerID;
    sendNetworkEventBroadcast(std::move(event));
}

void
jleSceneServer::sceneInspectorImGuiRender()
{
    networkSceneDisplayInspectorWindow("Server", sceneName, _server);
}

void
jleSceneServer::updateServerSceneObjects(float dt)
{
    JLE_SCOPE_PROFILE_CPU(jleScene_updateSceneObjects)
    for (int32_t i = _sceneObjects.size() - 1; i >= 0; i--) {
        if (_sceneObjects[i]->_pendingKill) {
            objectDestructionNetworked(_sceneObjects[i]);
            _sceneObjects[i]->propagateDestroy();
            _sceneObjects.erase(_sceneObjects.begin() + i);
            continue;
        }

        _sceneObjects[i]->updateComponentsServer(dt);
        _sceneObjects[i]->updateChildrenServer(dt);
    }
}

void
jleSceneServer::sendNetworkEventBroadcast(std::unique_ptr<jleServerToClientEvent> event)
{
    _eventsBroadcastQueue.push_back(std::move(event));
}

void
jleSceneServer::sendNetworkEventToUser(std::unique_ptr<jleServerToClientEvent> event, int32_t userId)
{
    _eventsSpecificUserQueue[userId].push_back(std::move(event));
}

void
jleSceneServer::objectDestructionNetworked(const std::shared_ptr<jleObject> &object)
{
    auto event = jleMakeNetEvent<jleDestroyObjectEvent>();
    event->objectNetId = object->netID();
    sendNetworkEventBroadcast(std::move(event));
}

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleSceneServer)

template <class Archive>
void
jleSceneServer::serialize(Archive &archive)
{
    archive(cereal::base_class<jleScene>(this));
}
