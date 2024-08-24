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

#include <plog/Log.h>
#include "serialization/jleBinaryArchive.h"

#include <memory>
#include <sstream>

struct jleServerToClientEvent;
struct jleClientToServerEvent;

class jleNetworkEventOutQueue
{
public:
    jleNetworkEventOutQueue();

    void enqueue(std::unique_ptr<jleServerToClientEvent> event);
    void enqueue(std::unique_ptr<jleClientToServerEvent> event);

    void
    resetQueue();

    [[nodiscard]] bool
    isEmpty() const;

    // Constructs the raw data used for event queue over the network.
    // First 4 bytes (int32) contains amount of events.
    [[nodiscard]] std::vector<uint8_t>
    data() const;

protected:
    std::ostringstream _eventOutputStream{std::ios::binary};
    std::unique_ptr<jleBinaryOutputArchive> _eventOutputArchive{nullptr};
    int32_t _queuedEvents{0};
};
