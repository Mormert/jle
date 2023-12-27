// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleCompileHelper.h"
#include "jleNetworkEventRegistrator.h"

struct jleNetworkEvent {
    virtual ~jleNetworkEvent() = default;
    virtual void execute() = 0;
};

struct jleClientToServerEvent : public jleNetworkEvent {
protected:
    friend class jleSceneServer;
    jleSceneServer &getSceneServer();

    int32_t getClientId();

private:
    jleSceneServer *_serverScene{};
    int32_t _clientId;
};

struct jleServerToClientEvent : public jleNetworkEvent {
protected:
    friend class jleSceneClient;
    jleSceneClient &getSceneClient();

private:
    jleSceneClient *_clientScene{};
    int64_t receivingClientId{};
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
