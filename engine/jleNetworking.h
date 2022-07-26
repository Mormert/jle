// Copyright (c) 2022. Johan Lind

#ifndef JLE_NETWORKING_H
#define JLE_NETWORKING_H

#ifndef __EMSCRIPTEN__

#include <sio_client.h>

#endif

#include "glm/vec3.hpp"
#include <json.hpp>
#include "readerwriterqueue/readerwriterqueue.h"

#include <string>
#include <array>

class jleNetworking {

public:

    jleNetworking() { sNet = this; }

    // Has native and JS implementation
    void Connect(const std::string &uri,
                 const std::function<void(void)>& onConnected,
                 const std::function<void(int const& reason)>& onClosed,
                 const std::function<void(void)>& onFailed);

    void Disconnect();

    static void TryEmitJsonData(const std::string &event, const nlohmann::json &json, const std::string& receiver = "");

    // Called from game thread
    void EmitJsonData(const std::string &event, const nlohmann::json &json, const std::string& receiver = "");

    // Called from game thread, returns false if no message packs are available
    bool TryReceiveMessagePack(nlohmann::json &out);

    static inline jleNetworking *sNet;

    static inline bool connected = false;

    static void OnReceiveMessagePackFromJS(const std::string& data);

#ifdef __EMSCRIPTEN__

    std::string id;
#endif

private:

    // Has native and JS implementation
    void EmitEvent(const std::string &event, const std::string &data);

    // Called from C++ or JavaScript, networking thread
    void OnReceiveMessagePack(const std::string& data);

    moodycamel::ReaderWriterQueue<nlohmann::json> mMessagePacks{128};

    // Has native and JS implementation
    std::string GetSessionID();

#ifndef __EMSCRIPTEN__
    sio::client c;
#endif
};

#endif // JLE_NETWORKING_H
