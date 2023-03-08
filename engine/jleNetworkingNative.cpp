// Copyright (c) 2023. Johan Lind

// Native code for the Networking class

#ifndef __EMSCRIPTEN__

#include "jleNetworking.h"
#include <iostream>

void jleNetworking::connect(
    const std::string &uri,
    const std::function<void(void)> &onConnected,
    const std::function<void(int const &reason)> &onClosed,
    const std::function<void(void)> &onFailed) {
    c.set_logs_quiet();

    c.set_open_listener(onConnected);
    c.set_close_listener(onClosed);
    c.set_fail_listener(onFailed);

    c.connect(uri);

    c.socket()->on(
        "msgpack",
        sio::socket::event_listener_aux([&](std::string const &name,
                                            sio::message::ptr const &data,
                                            bool isAck,
                                            sio::message::list &ack_resp) {
            onReceiveMessagePack(data->get_string());
        }));

    connected = true;
    sNet = this;
}

void jleNetworking::disconnect() {

    if (!connected) {
        return;
    }

    c.close();

    connected = false;
    sNet = nullptr;
}

void jleNetworking::emitJsonData(const std::string &event,
                                 const nlohmann::json &json,
                                 const std::string &receiver) {

    if (!connected) {
        return;
    }

    nlohmann::json json_package{{"e", event}, {"j", json}, {"r", receiver}};

    c.socket()->emit("msgpack", to_string(json_package));
}

void jleNetworking::emitEvent(const std::string &event,
                              const std::string &data) {

    if (!connected) {
        return;
    }

    c.socket()->emit(event, data);
}

std::string jleNetworking::sessionID() { return c.get_sessionid(); }

#endif // __EMSCRIPTEN__
