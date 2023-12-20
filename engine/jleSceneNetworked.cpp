// Copyright (c) 2023. Johan Lind

#include "jleSceneNetworked.h"

#include <enet.h>
#include <librg.h>

#include <utility>

void
jleSceneNetworked::trackEntityObject(librg_world *world, int64_t entityId, const std::shared_ptr<jleObject> &object)
{
    librg_entity_track(world, entityId);
    setNetEntityToObjectPointer(world, entityId, object);
}

void
jleSceneNetworked::trackEntityObject(int64_t entityId, const std::shared_ptr<jleObject> &object)
{
    trackEntityObject(_world, entityId, object);
}

void
jleSceneNetworked::untrackEntityObject(librg_world *world, int64_t entityId)
{
    resetNetEntityToObjectPointer(world, entityId);
    librg_entity_untrack(world, entityId);
}

void
jleSceneNetworked::untrackEntityObject(int64_t entityId)
{
    untrackEntityObject(_world, entityId);
}

void
jleSceneNetworked::resetNetEntityToObjectPointer(librg_world *world, int64_t entityId)
{
    auto *objectWeak = reinterpret_cast<std::weak_ptr<jleObject> *>(librg_entity_userdata_get(world, entityId));
    delete objectWeak;
    objectWeak = nullptr;
}

void
jleSceneNetworked::setNetEntityToObjectPointer(librg_world *world, int64_t entityId, std::weak_ptr<jleObject> object)
{
#ifndef NDEBUG
    auto previousValue = librg_entity_userdata_get(world, entityId);
    if (previousValue != nullptr) {
        LOGE << "Entity id " << entityId << " was already set, causing memory leak";
    }
#endif

    librg_entity_userdata_set(world, entityId, new std::weak_ptr<jleObject>(std::move(object)));
}

void
jleSceneNetworked::setNetEntityToObjectPointer(int64_t entityId, std::weak_ptr<jleObject> object)
{
    setNetEntityToObjectPointer(_world, entityId, std::move(object));
}

std::shared_ptr<jleObject>
jleSceneNetworked::getObjectPointerFromNetEntity(librg_world *world, int64_t entityId)
{
    auto *objectWeak = reinterpret_cast<std::weak_ptr<jleObject> *>(librg_entity_userdata_get(world, entityId));
    if (objectWeak->expired()) {
        return nullptr;
    }
    auto object = objectWeak->lock();
    if (object->pendingKill()) {
        return nullptr;
    }
    return object;
}

std::shared_ptr<jleObject>
jleSceneNetworked::getObjectPointerFromNetEntity(int64_t entityId)
{
    return getObjectPointerFromNetEntity(_world, entityId);
}

void
jleSceneNetworked::networkSceneDisplayInspectorWindow(const std::string &sceneType,
                                                      const std::string &sceneName,
                                                      ENetHost *host,
                                                      librg_world *world)
{

    uint16_t mySceneNetID;
    std::string otherSceneType;
    if (sceneType == "Server") {
        mySceneNetID = 0;
        otherSceneType = "Client";
    } else { // Else it's a "Client"
        mySceneNetID = host->peers[0].outgoingPeerID + 1;
        otherSceneType = "Server";
    }

    ImGui::Begin(std::string{sceneType + " for scene: " + sceneName}.c_str());

    ImGui::Text("Network debugging for: %s", sceneType.c_str());

    ImGui::Text("Scene network ID: %d", mySceneNetID);
    ImGui::Text("Incoming Bandwidth: %d", host->incomingBandwidth);
    ImGui::Text("Outgoing Bandwidth: %d", host->outgoingBandwidth);
    ImGui::Text("Allocated peers max: %zu", host->peerCount);
    ImGui::Text("Total data sent: %f MB", host->totalSentData / 1000000.f);

    if (ImGui::TreeNode("World Entities")) {
        int64_t entityIds[1024];
        size_t entitiesAmount;
        librg_world_fetch_all(world, entityIds, &entitiesAmount);
        for (int i = 0; i < entitiesAmount; i++) {
            auto entityId = entityIds[i];
            auto owner = librg_entity_owner_get(world, entityId);
            auto object = getObjectPointerFromNetEntity(world, entityId);
            if (!object) {
                continue;
            }
            if (owner == mySceneNetID) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            }
            ImGui::Text("Entity ID: %lld, owner : %lld, obj name: %s",
                        entityId,
                        object->netOwnerID(),
                        object->instanceName().c_str());
            ImGui::PopStyleColor();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode(std::string{sceneType + " Owned Objects"}.c_str())) {
        int64_t entityIds[1024];
        size_t entitiesAmount;
        librg_world_fetch_owner(world, mySceneNetID, entityIds, &entitiesAmount);
        for (int i = 0; i < entitiesAmount; i++) {
            auto entityId = entityIds[i];
            auto object = getObjectPointerFromNetEntity(world, entityId);
            if (!object) {
                continue;
            }
            ImGui::Text("Entity ID: %lld, obj name: %s", entityId, object->instanceName().c_str());
        }
        ImGui::TreePop();
    }

    {
        ENetPeer *currentPeer;
        for (currentPeer = host->peers; currentPeer < &host->peers[host->peerCount]; ++currentPeer) {
            uint16_t theirNetSceneID;
            if (sceneType == "Server") {
                theirNetSceneID = currentPeer->incomingPeerID + 1;
            } else {
                theirNetSceneID = currentPeer->incomingPeerID;
            }

            std::string connectStatus;
            if (currentPeer->state != ENET_PEER_STATE_CONNECTED) {
                connectStatus = " - disconnected";
            } else {
                connectStatus = " - connected";
            }
            if (ImGui::TreeNode(
                    std::string{otherSceneType + ' ' + std::to_string(currentPeer->incomingPeerID + 1) + connectStatus}
                        .c_str())) {
                if (currentPeer->state != ENET_PEER_STATE_CONNECTED) {
                    ImGui::Text("This peer is not connected.");
                } else {
                    if (ImGui::TreeNode("Stats")) {
                        ImGui::Text("Incoming peer id: %d", currentPeer->incomingPeerID);
                        ImGui::Text("Outgoing peer id: %d", currentPeer->outgoingPeerID);
                        if (sceneType == "Server") {
                            ImGui::Text("Scene Net id (client) : %d", theirNetSceneID);
                        } else {
                            ImGui::Text("Scene Net id (server) : %d", theirNetSceneID);
                        }
                        ImGui::Text("RTT: %d ms", currentPeer->roundTripTime);
                        ImGui::Text("Ping interval: %d ms", currentPeer->pingInterval);
                        ImGui::Text("Incoming data total : %f MB", currentPeer->incomingDataTotal / 1000000.f);
                        ImGui::Text("Outgoing data total : %f MB", currentPeer->outgoingDataTotal / 1000000.f);

                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Their Owned Objects")) {
                        int64_t entityIds[4096];
                        size_t entitiesAmount;
                        librg_world_fetch_all(world, entityIds, &entitiesAmount);
                        for (int i = 0; i < entitiesAmount; i++) {
                            auto entityId = entityIds[i];
                            auto object = getObjectPointerFromNetEntity(world, entityId);
                            if (object->netOwnerID() == theirNetSceneID) {
                                ImGui::Text("Entity ID: %lld, obj name: %s", entityId, object->instanceName().c_str());
                            }
                        }
                        ImGui::TreePop();
                    }
                }

                ImGui::TreePop();
            }
        }
    }

    ImGui::End();
}
