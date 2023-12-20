// Copyright (c) 2023. Johan Lind

#include "jleSceneServer.h"
#include "jleEngineSettings.h"
#include "jleProfiler.h"

#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>

#include <enet.h>
#include <librg.h>

int32_t
jleSceneServer::serverWriteUpdate(librg_world *w, librg_event *e)
{
    int64_t eventOwner = librg_event_owner_get(w, e);
    int64_t entityId = librg_event_entity_get(w, e);

    // prevent sending updates to users who own that entity
    // since they will be responsible on telling where that entity is supposed to be
    int64_t entityOwner = librg_entity_owner_get(w, entityId);
    if (entityOwner == eventOwner) {
        // return LIBRG_WRITE_REJECT;
    }

    auto object = getObjectPointerFromNetEntity(w, entityId);
    if (!object) {
        untrackEntityObject(w, entityId);
        return LIBRG_WRITE_REJECT;
    }
    if (object->pendingKill()) {
        untrackEntityObject(w, entityId);
        return LIBRG_WRITE_REJECT;
    }

    std::string string;
    try {
        std::ostringstream oss;
        { // Note that we need the archive to go out of scope to completely fill the string stream!
            cereal::JSONOutputArchive archive(oss);
            archive(*object);
        }
        string = oss.str();
    } catch (std::exception &e) {
        LOGE << "[server] failed parsing write data: " << e.what() << std::endl;
    }

    const char *data = string.data();
    const auto dataCount = string.size() + 1;

    char *buffer = librg_event_buffer_get(w, e);
    size_t max_length = librg_event_size_get(w, e);

    // check if we have enough space to write
    if (dataCount > max_length) {
        return LIBRG_WRITE_REJECT;
    }

    // write data and return how much we've written
    memcpy(buffer, data, dataCount);
    return dataCount;
}

int32_t
jleSceneServer::serverWriteCreate(librg_world *w, librg_event *e)
{
    int64_t eventOwner = librg_event_owner_get(w, e);
    int64_t entityId = librg_event_entity_get(w, e);

    int64_t entityOwner = librg_entity_owner_get(w, entityId);

    char *buffer = librg_event_buffer_get(w, e);

    size_t max_length = librg_event_size_get(w, e);
    constexpr size_t dataCount = sizeof(entityOwner);

    // check if we have enough space to write
    if (dataCount > max_length) {
        return LIBRG_WRITE_REJECT;
    }

    memcpy(buffer, &entityOwner, sizeof(dataCount));
    return dataCount;
}

int32_t
jleSceneServer::serverReadUpdate(librg_world *w, librg_event *e)
{
    int64_t entityId = librg_event_entity_get(w, e);
    size_t actual_length = librg_event_size_get(w, e);

    if (actual_length != sizeof(glm::vec3)) {
        LOGW << "[server] Invalid data size coming from client";
        return 0;
    }

    auto *peer = (ENetPeer *)librg_entity_userdata_get(w, entityId);
    char *buffer = librg_event_buffer_get(w, e);

    // read and update actual position
    glm::vec3 position{0.f};
    memcpy(peer->data, buffer, actual_length);
    memcpy(&position, buffer, actual_length);

    // and update librg actual chunk id
    librg_chunk chunk = librg_chunk_from_realpos(w, position.x, position.y, position.z);
    librg_entity_chunk_set(w, entityId, chunk);

    return 0;
}

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
    _world = librg_world_create();

    if (_world == nullptr) {
        LOGE << "[server] An error occurred while trying to create a server world.";
        return 1;
    }

    LOGI << "[server] Created a new server world";

    librg_world_userdata_set(_world, this);

    librg_config_chunksize_set(_world, 16, 16, 16);
    librg_config_chunkamount_set(_world, 9, 9, 9);
    librg_config_chunkoffset_set(_world, LIBRG_OFFSET_MID, LIBRG_OFFSET_MID, LIBRG_OFFSET_MID);

    librg_event_set(_world, LIBRG_WRITE_UPDATE, jleSceneServer::serverWriteUpdate);
    librg_event_set(_world, LIBRG_WRITE_CREATE, jleSceneServer::serverWriteCreate);
    // librg_event_set(_world, LIBRG_WRITE_CREATE, jleSceneServer::serverReadUpdate);

    spawnObjectWithName("server_dummy");

    return 0;
}

int
jleSceneServer::stopServer()
{
    if (!librg_world_valid(_world))
        return 1;

    // auto *server = (ENetHost *)librg_world_userdata_get(_server_world);

    enet_host_destroy(_server);
    librg_world_destroy(_world);
    _world = nullptr;

    LOGI << "[server] Stopped scene server: " << path.getVirtualPath();

    return 0;
}

jleSceneServer::~jleSceneServer()
{
    if (_world) {
        stopServer();
    }
}

void
jleSceneServer::updateScene(float dt)
{
    processNewSceneObjects();
    processNetwork();

    for (int32_t i = _sceneObjects.size() - 1; i >= 0; i--) {
        if (_sceneObjects[i]->_pendingKill) {
            _sceneObjects[i]->propagateDestroy();
            _sceneObjects.erase(_sceneObjects.begin() + i);
            continue;
        }
    }
}

void
jleSceneServer::onSceneStart()
{
    startServer();
}

void
jleSceneServer::onSceneDestruction()
{
    if (_world) {
        stopServer();
    }
}

void
jleSceneServer::processNetwork()
{
    JLE_SCOPE_PROFILE_CPU(jleSceneServer_processNetwork)

    if (!librg_world_valid(_world)) {
        return;
    }

    // auto *server = (ENetHost *)librg_world_userdata_get(_server_world);

    {
        JLE_SCOPE_PROFILE_CPU(jleSceneServer_updateScene_EnetService)

        ENetEvent event{};

        while (enet_host_service(_server, &event, 0) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                const auto incomingPlayerID = event.peer->incomingPeerID + 1;
                LOGI << "[server] A new user " << incomingPlayerID << " connected.";

                // Disable timeouts for the peer
                enet_peer_timeout(event.peer, 0, 0, 0);

                auto &thisPlayersObjects = _playerOwnedObjects[incomingPlayerID] = {};

                auto player = spawnObjectWithName("player_" + std::to_string(incomingPlayerID));
                player->_networkOwnerID = incomingPlayerID;
                thisPlayersObjects.push_back(player);

                librg_entity_owner_set(_world, player->netEntityID(), incomingPlayerID);

            } break;
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                const auto incomingPlayerID = event.peer->incomingPeerID + 1;
                LOGI << "[server] A user " << incomingPlayerID << " disconnected.";

                auto &thisPlayersObjects = _playerOwnedObjects[incomingPlayerID];
                for (auto &playerObject : thisPlayersObjects) {
                    if (auto &&sharedPtr = playerObject.lock()) {
                        untrackEntityObject(sharedPtr->netEntityID());
                        sharedPtr->destroyObject();
                    }
                }
            } break;

            case ENET_EVENT_TYPE_RECEIVE: {
                const auto incomingPlayerID = event.peer->incomingPeerID + 1;

                /* handle a newly received event */
                librg_world_read(
                    _world, incomingPlayerID, (char *)event.packet->data, event.packet->dataLength, nullptr);

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);
            } break;

            case ENET_EVENT_TYPE_NONE:
                break;
            }
        }
    }
    /* iterate peers and send them updates */
    ENetPeer *currentPeer;
    for (currentPeer = _server->peers; currentPeer < &_server->peers[_server->peerCount]; ++currentPeer) {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED) {
            continue;
        }

        char buffer[4096] = {0};
        size_t buffer_length = sizeof(buffer);

        const auto incomingPlayerID = currentPeer->incomingPeerID + 1;

        /* serialize peer's world view to a buffer */
        librg_world_write(_world,
                          incomingPlayerID,
                          2, /* chunk radius */
                          buffer,
                          &buffer_length,
                          nullptr);

        /* create packet with actual length, and send it */
        ENetPacket *packet = enet_packet_create(buffer, buffer_length, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(currentPeer, 0, packet);
    }
}

void
jleSceneServer::setupObject(const std::shared_ptr<jleObject> &obj)
{
    jleScene::setupObject(obj);
    const auto entityId = _entityIdGenerateCounter++;
    obj->_networkEntityID = entityId;
    obj->_networkOwnerID = serverOwnedId;

    trackEntityObject(entityId, obj);

    const auto position = obj->getTransform().getWorldPosition();
    librg_chunk chunk = librg_chunk_from_realpos(_world, position.x, position.y, position.z);

    librg_entity_chunk_set(_world, entityId, chunk);
    librg_entity_owner_set(_world, entityId, serverOwnedId);
}

jleSceneServer &
jleSceneServer::getSceneServerRef(librg_world *w)
{
    auto *c = (jleSceneServer *)librg_world_userdata_get(w);
    return *c;
}

void
jleSceneServer::sceneInspectorImGuiRender()
{
    networkSceneDisplayInspectorWindow("Server", sceneName, _server, _world);
}

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleSceneServer)

template <class Archive>
void
jleSceneServer::serialize(Archive &archive)
{
    archive(cereal::base_class<jleScene>(this));
}
