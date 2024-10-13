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

#include "jleSceneServer.h"
#include "jleEngineSettings.h"
#include "jleProfiler.h"
#include "jleSceneClient.h"
#include "modules/networking/jleNetworkEvent.h"

#include "serialization/jleBinaryArchive.h"
#include <cereal/cereal.hpp>

#include <enet.h>

struct jleCreateObjectEvent : public jleServerToClientEvent {
    void
    execute(jleEngineModulesContext& ctx) override
    {
        auto &scene = getSceneClient();
        if (newObject) {
            scene.spawnObjectFromServer(ctx, newObject, objectNetId, objectOwnerId);
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
    execute(jleEngineModulesContext& ctx) override
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
    execute(jleEngineModulesContext& ctx) override
    {
        auto &scene = getSceneClient();
        for (int i = 0; i < objects.size(); i++) {
            scene.spawnObjectFromServer(ctx, objects[i], objectsNetId[i], objectsOwner[i]);
        }
    }

    template <class Archive>
    void
    serialize(Archive &archive)
    {
        archive(CEREAL_NVP(objects), CEREAL_NVP(objectsNetId), CEREAL_NVP(objectsOwner));
    }

    std::vector<std::shared_ptr<jleObject>> objects;
    std::vector<int16_t> objectsNetId;
    std::vector<int16_t> objectsOwner;
};

JLE_REGISTER_NET_EVENT(jleFullSceneSyncEvent)

int
jleSceneServer::startServer(jleEngineModulesContext& ctx, int port, int maxClients)
{
    ENetAddress address = {0};

    address.host = ENET_HOST_ANY;
    address.port = port;

    _server = enet_host_create(&address, maxClients, 2, 0, 0);

    ENetCompressor compressor;
    initializeENetCompressor(compressor);
    enet_host_compress(_server, &compressor);

    if (_server == nullptr) {
        LOGE << "[server] An error occurred while trying to create an ENet server host.";
        return 1;
    }

    LOGI << "[server] Started an ENet server...";

    for (auto &object : _sceneObjects) {
        setupObjectForNetworking(object);
    }

    jleSerializationContext serializationContext{&ctx.resourcesModule, &ctx.luaEnvironment, &ctx.renderThread};

    spawnObjectWithName("server_dummy", serializationContext);

    return 0;
}

int
jleSceneServer::stopServer()
{
    if (_server) {
        enet_host_destroy(_server);
        _server = nullptr;
    }

    LOGI << "[server] Stopped scene server: " << path.getVirtualPath();

    return 0;
}

jleSceneServer::~
jleSceneServer()
{
    stopServer();
}

void
jleSceneServer::updateScene(jleEngineModulesContext& ctx)
{
    processNewSceneObjects(ctx);
    updateServerSceneObjects(ctx);

    processNetwork(ctx);
}

void
jleSceneServer::onSceneStart(jleEngineModulesContext& ctx)
{
    startServer(ctx);
}

void
jleSceneServer::onSceneDestruction()
{
    stopServer();
}

void
jleSceneServer::processNetwork(jleEngineModulesContext& ctx)
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

                auto syncEvent = jleMakeNetEvent<jleFullSceneSyncEvent>();
                syncEvent->objects = _sceneObjects;
                for (auto &object : _sceneObjects) {
                    syncEvent->objectsNetId.push_back(object->netID());
                    syncEvent->objectsOwner.push_back(object->netOwnerID());
                }
                sendNetworkEventToUser(std::move(syncEvent), incomingPlayerID);

                onClientConnect(ctx, incomingPlayerID);

            } break;
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                const auto incomingPlayerID = event.peer->incomingPeerID + 1;
                LOGI << "[server] A user " << incomingPlayerID << " disconnected.";

                destroyAllClientOwnedObjects(incomingPlayerID);
                onClientDisconnect(incomingPlayerID);
            } break;

            case ENET_EVENT_TYPE_RECEIVE: {
                const auto incomingPlayerID = event.peer->incomingPeerID + 1;
                const char *dataBuffer = reinterpret_cast<char *>(event.packet->data);
                const auto dataLength = event.packet->dataLength;

                jleExecuteClientEventsOnServer(ctx, dataBuffer, dataLength, this, incomingPlayerID);

                // Clean up the packet now that we're done using it.
                enet_packet_destroy(event.packet);
            } break;

            case ENET_EVENT_TYPE_NONE:
                break;
            }
        }
    }

    if (!_eventsBroadcastQueue.isEmpty()) {
        const auto data = _eventsBroadcastQueue.data();

        auto *packetBuffer = &data[0];
        const size_t packetBufferLen = data.size();

        ENetPacket *packet = enet_packet_create(packetBuffer, packetBufferLen, ENET_PACKET_FLAG_RELIABLE);
        for (ENetPeer *currentPeer = _server->peers; currentPeer < &_server->peers[_server->peerCount]; ++currentPeer) {
            if (currentPeer->state != ENET_PEER_STATE_CONNECTED) {
                continue;
            }

            enet_peer_send(currentPeer, 0, packet);
        }
        _eventsBroadcastQueue.resetQueue();
    }

    if (!_eventsSpecificUserQueue.empty()) {
        for (auto &[clientId, outQueue] : _eventsSpecificUserQueue) {
            const auto peerId = clientId - 1;
            const auto data = outQueue.data();

            auto *packetBuffer = &data[0];
            const size_t packetBufferLen = data.size();

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

std::shared_ptr<jleObject>
jleSceneServer::getObjectFromNetId(int32_t netId)
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
jleSceneServer::setupObject(const std::shared_ptr<jleObject> &obj, jleSerializationContext& ctx)
{
    jleScene::setupObject(obj, ctx);
    setupObjectForNetworking(obj);
}

void
jleSceneServer::setupObjectForNetworking(const std::shared_ptr<jleObject> &obj)
{
    const auto entityId = _entityIdGenerateCounter++;
    obj->_netId = entityId;
    if (obj->_networkOwnerID == -1) {
        obj->_networkOwnerID = serverOwnedId;
    }
    obj->propagateOwnedBySceneServer(this);

    setNetIdObject(obj, entityId);

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

std::shared_ptr<jleObject>
jleSceneServer::spawnObjectWithOwner(jleEngineModulesContext& ctx, const std::string &objectName, int32_t ownerId)
{
    auto newSceneObject = std::make_shared<jleObject>();
    newSceneObject->_networkOwnerID = ownerId;

    jleSerializationContext serializationContext{&ctx.resourcesModule, &ctx.luaEnvironment, &ctx.renderThread};

    setupObject(newSceneObject, serializationContext);
    newSceneObject->_instanceName = objectName;

    if (ownerId > 0) {
        auto &thisPlayersObjects = _playerOwnedObjects[ownerId];
        thisPlayersObjects.push_back(newSceneObject);
    }

    return newSceneObject;
}

void
jleSceneServer::updateServerSceneObjects(jleEngineModulesContext& ctx)
{
    JLE_SCOPE_PROFILE_CPU(jleScene_updateSceneObjects)
    for (int32_t i = _sceneObjects.size() - 1; i >= 0; i--) {
        if (_sceneObjects[i]->_pendingKill) {
            objectDestructionNetworked(_sceneObjects[i]);
            _sceneObjects[i]->propagateDestroy(ctx);
            _sceneObjects.erase(_sceneObjects.begin() + i);
            continue;
        }

        _sceneObjects[i]->updateComponentsServer(ctx);
        _sceneObjects[i]->updateChildrenServer(ctx);
    }
}

void
jleSceneServer::sendNetworkEventBroadcast(std::unique_ptr<jleServerToClientEvent> event)
{
    _eventsBroadcastQueue.enqueue(std::move(event));
}

void
jleSceneServer::sendNetworkEventToUser(std::unique_ptr<jleServerToClientEvent> event, int32_t userId)
{
    _eventsSpecificUserQueue[userId].enqueue(std::move(event));
}

void
jleSceneServer::objectDestructionNetworked(const std::shared_ptr<jleObject> &object)
{
    auto event = jleMakeNetEvent<jleDestroyObjectEvent>();
    event->objectNetId = object->netID();
    sendNetworkEventBroadcast(std::move(event));
}
void
jleSceneServer::destroyAllClientOwnedObjects(int32_t clientId)
{
    auto &thisPlayersObjects = _playerOwnedObjects[clientId];
    for (auto &playerObject : thisPlayersObjects) {
        if (auto &&object = playerObject.lock()) {
            object->destroyObject();
        }
    }
}

void
jleSceneServer::setNetIdObject(const std::shared_ptr<jleObject> &object, int32_t netId)
{
    _networkedObjects.insert(std::make_pair(netId, object));
}

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleSceneServer)

template <class Archive>
void
jleSceneServer::serialize(Archive &archive)
{
    archive(cereal::base_class<jleScene>(this));
}
