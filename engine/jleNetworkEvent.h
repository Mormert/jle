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

template <typename EventBase>
void jleExecuteNetEvents(const char *networkBuffer, size_t networkBufferLength, jleScene *scene, int clientId = 0);

struct jleClientToServerEvent : public jleNetworkEvent {
protected:
    friend class jleSceneServer;
    friend void jleExecuteNetEvents<jleClientToServerEvent>(const char *networkBuffer, size_t networkBufferLength, jleScene *scene, int clientId);
    jleSceneServer &getSceneServer();

    int32_t getClientId();

private:
    jleSceneServer *_serverScene{};
    int32_t _clientId;
};

struct jleServerToClientEvent : public jleNetworkEvent {
protected:
    friend class jleSceneClient;
    friend void jleExecuteNetEvents<jleServerToClientEvent>(const char *networkBuffer, size_t networkBufferLength, jleScene *scene, int clientId);

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

template <typename Event>
void
jleExecuteNetEvents(const char *networkBuffer, size_t networkBufferLength, jleScene *scene, int clientId)
{
    static_assert(std::is_same<jleServerToClientEvent, Event>::value ||
                      std::is_same<jleClientToServerEvent, Event>::value,
                  "EventBase must be either jleServerToClientEvent or jleClientToServerEvent");

    int32_t amountOfEvents;
    ::memcpy(&amountOfEvents, &networkBuffer[0], sizeof(int32_t));

    std::stringstream stream{};
    stream.write(&networkBuffer[sizeof(int32_t)], networkBufferLength - sizeof(int32_t));

    cereal::BinaryInputArchive archive(stream);

    for (int i = 0; i < amountOfEvents; i++) {
        std::unique_ptr<Event> e{nullptr};
        try {
            archive(e);
            if (e) {
                if constexpr (std::is_same<jleServerToClientEvent, Event>::value) {
                    e->_clientScene = reinterpret_cast<jleSceneClient*>(scene);
                } else if constexpr (std::is_same<jleClientToServerEvent, Event>::value) {
                    e->_serverScene = reinterpret_cast<jleSceneServer*>(scene);
                    e->_clientId = clientId;
                }
                e->execute();
            }
        } catch (std::exception &ex) {
            LOGE << "[client] failed to parse event data: " << ex.what();
        }
    }
}

template <typename EventBase>
class jleNetworkEventOutQueue final
{
public:
    jleNetworkEventOutQueue() { resetQueue(); }

    void
    enqueue(std::unique_ptr<EventBase> event)
    {
        static_assert(std::is_base_of<jleServerToClientEvent, EventBase>::value ||
                          std::is_base_of<jleClientToServerEvent, EventBase>::value,
                      "Event must derive from either jleServerToClientEvent or jleClientToServerEvent");

        // Immediately serialize to binary, stores result in stringstream
        try {
            _eventOutputArchive->operator()(event);
            _queuedEvents++;
        } catch (std::exception &e) {
            LOGE << "Failed to write event data: " << e.what();
        }
    }

    void
    resetQueue()
    {
        _eventOutputStream = std::ostringstream{std::ios::binary};
        _eventOutputArchive = std::make_unique<cereal::BinaryOutputArchive>(_eventOutputStream);
        _queuedEvents = 0;
    }

    [[nodiscard]] bool
    isEmpty() const
    {
        return _queuedEvents <= 0;
    }

    // Constructs the raw data used for event queue over the network.
    // Data will be invalid after peek. First 4 bytes (int32) contains amount of events.
    [[nodiscard]] std::vector<uint8_t>
    data() const
    {
        std::vector<uint8_t> vec;

        const auto &str = _eventOutputStream.str();

        vec.resize(sizeof(_queuedEvents) + str.size());

        ::memcpy(&vec[0], &_queuedEvents, sizeof(_queuedEvents));
        std::copy(str.begin(), str.end(), vec.begin() + sizeof(_queuedEvents));

        return vec;
    }

private:
    std::ostringstream _eventOutputStream{std::ios::binary};
    std::unique_ptr<cereal::BinaryOutputArchive> _eventOutputArchive{nullptr};
    int32_t _queuedEvents{0};
};
