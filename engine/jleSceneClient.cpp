// Copyright (c) 2023. Johan Lind

#include "jleSceneClient.h"
#include "jleProfiler.h"

#include <enet.h>
#include <librg.h>

int32_t
jleSceneClient::clientReadCreate(librg_world *w, librg_event *e)
{
    int64_t ownerId = librg_event_owner_get(w, e);
    int64_t entityId = librg_event_entity_get(w, e);
    size_t actualLength = librg_event_size_get(w, e);

    const size_t bufferLen = actualLength - 1; // we need to remove 1 here if we're dealing with text-based data (with
                                               // null terminating character)

    char *buffer = librg_event_buffer_get(w, e);

    std::string bufferAsString(buffer, bufferLen);

    std::stringstream stream{};
    stream << bufferAsString;

    auto &scene = getSceneClientRef(w);

    EntityCreate entityCreate;

    try {
        cereal::JSONInputArchive inputArchive(stream);
        inputArchive(entityCreate);
    } catch (std::exception &e) {
        LOGE << "[client] failed parsing read data: " << e.what() << std::endl;
        return 0;
    }

    entityCreate.object->_networkEntityID = entityId;
    entityCreate.object->_networkOwnerID = entityCreate.entityOwner;
    entityCreate.object->propagateOwnedByScene(&scene);
    scene._sceneObjects.push_back(entityCreate.object);

    setNetEntityToObjectPointer(w, entityId, entityCreate.object);
    librg_entity_owner_set(w, entityId, entityCreate.entityOwner);

    LOGI << "[client] An entity " << (int)entityId << " from owner " << (int)ownerId << " was created.";

    return 0;
}

int32_t
jleSceneClient::clientReadRemove(librg_world *w, librg_event *e)
{
    int64_t ownerId = librg_event_owner_get(w, e);
    int64_t entityId = librg_event_entity_get(w, e);
    LOGI << "[client] Entity " << (int)entityId << " was removed for owner: " << (int)ownerId;

    auto obj = getObjectPointerFromNetEntity(w, entityId);
    if (obj) {
        if (!obj->pendingKill()) {
            obj->destroyObject();
        }
    }

    resetNetEntityToObjectPointer(w, entityId);
    return 0;
}

int32_t
jleSceneClient::clientReadUpdate(librg_world *w, librg_event *e)
{
    return 0;

    int64_t entityId = librg_event_entity_get(w, e);
    size_t actualLength = librg_event_size_get(w, e);

    char *buffer = librg_event_buffer_get(w, e);

    std::shared_ptr<jleObject> object = getObjectPointerFromNetEntity(w, entityId);

    if (!object) {
        LOGE << "[client] tried updating object with entity id " << entityId << ", but the object does not exist.";
        return 0;
    }

    const size_t bufferLen = actualLength - 1; // we need to remove 1 here if we're dealing with text-based data (with
                                               // null terminating character)

    std::string bufferAsString(buffer, bufferLen);

    try {
        ComponentsInEntityUpdate componentsInEntityUpdate;
        {
            std::stringstream stream{};
            stream << bufferAsString;
            cereal::JSONInputArchive inputArchive(stream);
            inputArchive(componentsInEntityUpdate);
        }

        for (const auto &componentUpdate : componentsInEntityUpdate) {
            std::stringstream componentStream{};
            componentStream.write(&componentUpdate.data[0], componentUpdate.data.size());

            cereal::BinaryInputArchive componentArchive(componentStream);

            auto component = object->_components[componentUpdate.componentIndex];
            component->netSyncIn(componentArchive);
        }
    } catch (std::exception &e) {
        LOGE << "Failed to parse component update data: " << e.what();
    }

    return 0;
}

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

    _world = librg_world_create();
    librg_world_userdata_set(_world, this);

    librg_event_set(_world, LIBRG_READ_REMOVE, jleSceneClient::clientReadRemove);
    librg_event_set(_world, LIBRG_READ_CREATE, jleSceneClient::clientReadCreate);
    librg_event_set(_world, LIBRG_READ_UPDATE, jleSceneClient::clientReadUpdate);
}

void
jleSceneClient::disconnectFromServer()
{
    if (!librg_world_valid(_world)) {
        LOGE << "[client] Failed to disconnect from server";
        return;
    }

    librg_world_destroy(_world);
    _world = nullptr;

    enet_peer_disconnect_now(_peer, 0);
    enet_host_destroy(_client);

    LOGI << "[client] Disconnected scene client: " << path.getVirtualPath();
}

jleSceneClient::~jleSceneClient()
{
    if (_world) {
        disconnectFromServer();
    }
}

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
    if (_world) {
        disconnectFromServer();
    }
}
void
jleSceneClient::processNetwork()
{
    JLE_SCOPE_PROFILE_CPU(jleSceneClientProcessNetwork)

    if (!librg_world_valid(_world)) {
        return;
    }

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
                librg_world_read(_world, localClientSceneId, (char *)dataBuffer, dataLength, nullptr);
            } break;
            }

            enet_packet_destroy(event.packet);
        } break;
        case ENET_EVENT_TYPE_NONE:
            break;
        }
    }

    {
        char packetBuffer[1024] = {0};

        // Set op code as first byte
        packetBuffer[0] = static_cast<char>(jleNetOpCode::WorldWrite);

        // Account for first byte being the op code
        auto *writeBuffer = &packetBuffer[1];
        size_t bufferLen = sizeof(packetBuffer) - 1;

        // serialize peer's world view to a buffer
        librg_world_write(_world, localClientSceneId, 0, writeBuffer, &bufferLen, nullptr);

        const size_t packetBufferLen = bufferLen + 1;

        ENetPacket *packet = enet_packet_create(packetBuffer, packetBufferLen, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(_peer, 0, packet);
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

jleSceneClient &
jleSceneClient::getSceneClientRef(librg_world *w)
{
    auto *c = (jleSceneClient *)librg_world_userdata_get(w);
    return *c;
}

void
jleSceneClient::sceneInspectorImGuiRender()
{
    networkSceneDisplayInspectorWindow("Client", sceneName, _client, _world);
}

void
jleSceneClient::sendNetworkEvent(std::unique_ptr<jleClientToServerEvent> event)
{
    _eventsQueue.push_back(std::move(event));
}

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleSceneClient)

template <class Archive>
void
jleSceneClient::serialize(Archive &archive)
{
    archive(cereal::base_class<jleScene>(this));
}
