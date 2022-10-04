// Copyright (c) 2022. Johan Lind

// Shared code between JS and Native for the Networking class

#include "jleNetworking.h"

void jleNetworking::onReceiveMessagePack(const std::string& data) {
    const nlohmann::json json = nlohmann::json::parse(data);
    _messagePacks.enqueue(json);
}

bool jleNetworking::tryReceiveMessagePack(nlohmann::json& out) {
    return _messagePacks.try_dequeue(out);
}

void jleNetworking::tryEmitJsonData(const std::string& event,
                                    const nlohmann::json& json,
                                    const std::string& receiver) {
    if (!sNet) {
        return;
    }

    sNet->emitJsonData(event, json, receiver);
}
