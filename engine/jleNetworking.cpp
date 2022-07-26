// Copyright (c) 2022. Johan Lind

// Shared code between JS and Native for the Networking class

#include "jleNetworking.h"

void jleNetworking::OnReceiveMessagePack(const std::string &data) {
    const nlohmann::json json = nlohmann::json::parse(data);
    mMessagePacks.enqueue(json);
}

bool jleNetworking::TryReceiveMessagePack(nlohmann::json &out) {
    return mMessagePacks.try_dequeue(out);
}

void jleNetworking::TryEmitJsonData(const std::string &event, const nlohmann::json &json, const std::string &receiver) {
    if (!sNet) {
        return;
    }

    sNet->EmitJsonData(event, json, receiver);
}
