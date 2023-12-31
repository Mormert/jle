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

#include "jleSceneNetworked.h"

#include <enet.h>
#include <zlib/zlib.h>

#undef min
#undef max

void
jleSceneNetworked::networkSceneDisplayInspectorWindow(const std::string &sceneType,
                                                      const std::string &sceneName,
                                                      ENetHost *host)
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

    if (!host) {
        ImGui::Text("The %s is not connected/started.", sceneType.c_str());
        ImGui::End();
        return;
    }

    ImGui::Text("Scene network ID: %d", mySceneNetID);
    ImGui::Text("Incoming Bandwidth: %d", host->incomingBandwidth);
    ImGui::Text("Outgoing Bandwidth: %d", host->outgoingBandwidth);
    ImGui::Text("Allocated peers max: %zu", host->peerCount);
    ImGui::Text("Total data sent: %f MB", host->totalSentData / 1000000.f);

    /* if (ImGui::TreeNode("World Entities")) {
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
     }*/

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

                    /*  if (ImGui::TreeNode("Their Owned Objects")) {
                          int64_t entityIds[4096];
                          size_t entitiesAmount;
                          librg_world_fetch_all(world, entityIds, &entitiesAmount);
                          for (int i = 0; i < entitiesAmount; i++) {
                              auto entityId = entityIds[i];
                              auto object = getObjectPointerFromNetEntity(world, entityId);
                              if (object->netOwnerID() == theirNetSceneID) {
                                  ImGui::Text("Entity ID: %lld, obj name: %s", entityId,
                      object->instanceName().c_str());
                              }
                          }
                          ImGui::TreePop();
                      }*/
                }

                ImGui::TreePop();
            }
        }
    }

    ImGui::End();
}

static std::vector<uint8_t> testBuffer;

class jleNetworkCompression
{
public:
    static size_t
    compressData(void *context,
                 const ENetBuffer *inBuffers,
                 size_t inBufferCount,
                 size_t inLimit,
                 uint8_t *outData,
                 size_t outLimit)
    {
        auto *compressor = (jleSceneNetworked *)(context);

        if (compressor->_compressionSrcMemory.size() < inLimit) {
            compressor->_compressionSrcMemory.resize(inLimit);
        }

        int total = inLimit;
        int ofs = 0;
        while (total) {
            for (size_t i = 0; i < inBufferCount; i++) {
                const int to_copy = std::min(total, int(inBuffers[i].dataLength));

                ::memcpy(&compressor->_compressionSrcMemory[ofs], inBuffers[i].data, to_copy);

                ofs += to_copy;
                total -= to_copy;
            }
        }

        const auto req_size = ::compressBound(ofs);
        if (compressor->_compressionDstMemory.size() < req_size) {
            compressor->_compressionDstMemory.resize(req_size);
        }

        uLongf compressedLen = compressor->_compressionDstMemory.size();
        int ret = ::compress(
            &compressor->_compressionDstMemory[0], &compressedLen, &compressor->_compressionSrcMemory[0], ofs);

        if (ret != Z_OK) {
            return 0;
        }

        if (compressedLen > outLimit) {
            // When this happens, we don't send compressed package (happens on small data, ~10's of bytes or so)
            return 0;
        }

        ::memcpy(outData, &compressor->_compressionDstMemory[0], compressedLen);

        return compressedLen;
    }

    static size_t
    decompressData(void *context, const uint8_t *inData, size_t inLimit, uint8_t *outData, size_t outLimit)
    {
        uLongf uncompressedLen = outLimit;
        int ret = ::uncompress(outData, &uncompressedLen, inData, inLimit);

        if (ret != Z_OK) {
            LOGE << "Failed to decompress net package";
            return 0;
        }

        return uncompressedLen;
    }
};

void
jleSceneNetworked::initializeENetCompressor(ENetCompressor &compressor)
{
    compressor.context = this;
    compressor.compress = jleNetworkCompression::compressData;
    compressor.decompress = jleNetworkCompression::decompressData;
    compressor.destroy = nullptr;
}
