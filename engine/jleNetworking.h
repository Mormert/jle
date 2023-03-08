// Copyright (c) 2023. Johan Lind

#ifndef JLE_NETWORKING_H
#define JLE_NETWORKING_H

#ifndef __EMSCRIPTEN__

#include <sio_client.h>

#endif

#include "glm/vec3.hpp"
#include "readerwriterqueue/readerwriterqueue.h"
#include <json.hpp>

#include <array>
#include <string>

class jleNetworking {

public:
    jleNetworking() { sNet = this; }

    // Has native and JS implementation
    void connect(const std::string &uri,
                 const std::function<void(void)> &onConnected,
                 const std::function<void(int const &reason)> &onClosed,
                 const std::function<void(void)> &onFailed);

    void disconnect();

    static void tryEmitJsonData(const std::string &event,
                                const nlohmann::json &json,
                                const std::string &receiver = "");

    // Called from game thread
    void emitJsonData(const std::string &event,
                      const nlohmann::json &json,
                      const std::string &receiver = "");

    // Called from game thread, returns false if no message packs are available
    bool tryReceiveMessagePack(nlohmann::json &out);

    static inline jleNetworking *sNet;

    static inline bool connected = false;

    static void onReceiveMessagePackFromJS(const std::string &data);

#ifdef __EMSCRIPTEN__

    std::string id;
#endif

private:
    // Has native and JS implementation
    void emitEvent(const std::string &event, const std::string &data);

    // Called from C++ or JavaScript, networking thread
    void onReceiveMessagePack(const std::string &data);

    moodycamel::ReaderWriterQueue<nlohmann::json> _messagePacks{128};

    // Has native and JS implementation
    std::string sessionID();

#ifndef __EMSCRIPTEN__
    sio::client c;
#endif
};

#endif // JLE_NETWORKING_H
