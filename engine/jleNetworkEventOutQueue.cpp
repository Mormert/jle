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

#include "jleNetworkEventOutQueue.h"
#include "jleNetworkEvent.h"

jleNetworkEventOutQueue::jleNetworkEventOutQueue() { resetQueue(); }

void
jleNetworkEventOutQueue::resetQueue()
{
    _eventOutputStream = std::ostringstream{std::ios::binary};

    jleSerializationContext ctx{nullptr, nullptr};
    _eventOutputArchive = std::make_unique<jleBinaryOutputArchive>(_eventOutputStream, ctx);
    _queuedEvents = 0;
}

bool
jleNetworkEventOutQueue::isEmpty() const
{
    return _queuedEvents <= 0;
}

std::vector<uint8_t>
jleNetworkEventOutQueue::data() const
{
    std::vector<uint8_t> vec;

    const auto &str = _eventOutputStream.str();

    vec.resize(sizeof(_queuedEvents) + str.size());

    ::memcpy(&vec[0], &_queuedEvents, sizeof(_queuedEvents));
    std::copy(str.begin(), str.end(), vec.begin() + sizeof(_queuedEvents));

    return vec;
}

void
jleNetworkEventOutQueue::enqueue(std::unique_ptr<jleServerToClientEvent> event)
{
    try {
        _eventOutputArchive->operator()(event);
        _queuedEvents++;
    } catch (std::exception &e) {
        LOGE << "Failed to write server to client event data: " << e.what();
    }
}

void
jleNetworkEventOutQueue::enqueue(std::unique_ptr<jleClientToServerEvent> event)
{
    try {
        _eventOutputArchive->operator()(event);
        _queuedEvents++;
    } catch (std::exception &e) {
        LOGE << "Failed to write client to server event data: " << e.what();
    }
}
