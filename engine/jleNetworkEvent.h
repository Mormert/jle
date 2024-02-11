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

#include "jleNetworkEventRegistrator.h"

#include <cereal/archives/binary.hpp>
#include <plog/Log.h>
#include <sstream>

struct jleNetworkEvent {
    virtual ~jleNetworkEvent() = default;
    virtual void execute() = 0;
};

class jleScene;
class jleSceneServer;
class jleSceneClient;

struct jleClientToServerEvent : public jleNetworkEvent {
protected:
    friend void jleExecuteClientEventsOnServer(const char *networkBuffer, size_t networkBufferLength, jleSceneServer *scene, int clientId);
    friend class jleSceneServer;
    jleSceneServer &getSceneServer();

    int32_t getClientId();

private:
    jleSceneServer *_serverScene{};
    int32_t _clientId;
};

struct jleServerToClientEvent : public jleNetworkEvent {
protected:
    friend void jleExecuteServerEventsOnClient(const char *networkBuffer, size_t networkBufferLength, jleSceneClient* scene);
    friend class jleSceneClient;

    jleSceneClient &getSceneClient();

private:
    jleSceneClient *_clientScene{};
};

JLE_REGISTER_NET_EVENT(jleClientToServerEvent)
JLE_REGISTER_NET_EVENT(jleServerToClientEvent)

template <typename Event>
std::unique_ptr<Event>
jleMakeNetEvent()
{
    static_assert(std::is_base_of<jleNetworkEvent, Event>::value, "Event must derive from jleNetworkEvent");
    return std::make_unique<Event>();
}

void jleExecuteClientEventsOnServer(const char *networkBuffer, size_t networkBufferLength, jleSceneServer* scene, int clientId);
void jleExecuteServerEventsOnClient(const char *networkBuffer, size_t networkBufferLength, jleSceneClient* scene);
