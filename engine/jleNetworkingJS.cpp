// Copyright (c) 2023. Johan Lind

// JavaScript glue code for the Networking class

// clang-format off

#ifdef __EMSCRIPTEN__

#include "jleNetworking.h"

#include <emscripten.h>

#include <string>
#include <regex>
#include <iostream>

static std::function<void(void)> sNetworkingJs_onConnected;
static std::function<void(int const &reason)> sNetworkingJs_onClosed;
static std::function<void(void)> sNetworkingJs_onFailed;

extern "C"
{

EMSCRIPTEN_KEEPALIVE
void on_receive_message_pack(char *value) {
    const std::string dataString{value};
    jleNetworking::onReceiveMessagePackFromJS(dataString);
}

// TODO: Update connected variable on this event instead of directly in connect()
EMSCRIPTEN_KEEPALIVE
void on_socket_connect(char *value) {
    const std::string dataString{value};
    sNetworkingJs_onConnected();
}

EMSCRIPTEN_KEEPALIVE
void on_socket_disconnect() {
    sNetworkingJs_onClosed(0);
}

EMSCRIPTEN_KEEPALIVE
void on_socket_failed() {
    sNetworkingJs_onFailed();
}
}

void jleNetworking::onReceiveMessagePackFromJS(const std::string &data) {
    if (!jleNetworking::sNet) {
        return;
    }

    sNet->onReceiveMessagePack(data);
}


void jleNetworking::connect(const std::string &uri,
                            const std::function<void(void)> &onConnected,
                            const std::function<void(int const &reason)> &onClosed,
                            const std::function<void(void)> &onFailed) {

    sNetworkingJs_onConnected = onConnected;
    sNetworkingJs_onClosed = onClosed;
    sNetworkingJs_onFailed = onFailed;

    // We need to connect with https
    std::string uri_https = std::regex_replace(uri, std::regex("http"), "https");

    // Note that we set the socket on the window object, such that it will be globally accessible.
    const std::string js_eval_string = "window.socket = io(\"" + uri_https +
                                       "\", {transports: ['websocket']}); console.log(\"Socket connecting...\");";
    emscripten_run_script(js_eval_string.c_str());

    EM_ASM(
        window.socket.on('msgpack', data => {
                stringData = data;
                var ptr = allocate(intArrayFromString(stringData), ALLOC_NORMAL);

                // Pass the string over to C++
                _on_receive_message_pack(ptr);

                // Free the data string
                _free(ptr);
            });

        window.socket.on("connect", () => {
                stringData = window.socket.id;
                var ptr = allocate(intArrayFromString(stringData), ALLOC_NORMAL);

                // Pass the string over to C++
                _on_socket_connect(ptr);

                // Free the data string
                _free(ptr);

                console.log("Socket connected!");
            });

        window.socket.on('disconnect', () => {
                _on_socket_disconnect();
            });

        window.socket.on('connect_error', data => {
                _on_socket_failed();
            });

    );

    connected = true;
    sNet = this;

}

void jleNetworking::disconnect() {

    if (!connected) {
        return;
    }

    EM_ASM(
        window.socket.disconnect();
    );

    connected = false;
    sNet = nullptr;
}

void jleNetworking::emitJsonData(const std::string &event, const nlohmann::json &json, const std::string &receiver) {

    if (!connected) {
        return;
    }

    nlohmann::json json_package{{"e", event},
                                {"j", json},
                                {"r", receiver}};

    std::string js_eval_string = "window.socket.emit(\"msgpack\", " + to_string(json_package) + ");";

    emscripten_run_script(js_eval_string.c_str());
}

void jleNetworking::emitEvent(const std::string &event, const std::string &data) {

    if (!connected) {
        return;
    }

    std::string js_eval_string = "window.socket.emit(\"" + event + "\", \"" + data + "\");";

    // We must replace all \n with \\n for it to not fail eval execution in JavaScript.
    js_eval_string = std::regex_replace(js_eval_string, std::regex("\n"), "\\n");

    emscripten_run_script(js_eval_string.c_str());
}

std::string jleNetworking::sessionID() {

    if (!connected) {
        return "";
    }

    return id;
}

#endif // __EMSCRIPTEN__