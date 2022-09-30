// Copyright (c) 2022. Johan Lind

#include "jleNetScene.h"
#include "jleCore.h"
#include "plog/Log.h"

jleNetScene::jleNetScene() : jleScene() {}

jleNetScene::jleNetScene(const std::string& sceneName) : jleScene(sceneName) {}

void jleNetScene::ToJson(nlohmann::json& j_out) {
    jleScene::ToJson(j_out);
    j_out["_connectAddress"] = _connectAddress;

    if (_isHost) {
        _networking.EmitJsonData("SceneSync", j_out);
    }
}

void jleNetScene::FromJson(const nlohmann::json& j_in) {
    jleScene::FromJson(j_in);
    JLE_FROM_JSON_IF_EXISTS(j_in, _connectAddress, "_connectAddress");
}

void jleNetScene::SceneUpdate() {
    nlohmann::json j;
    // Sync data back to game thread, and process the network message
    if (_networking.TryReceiveMessagePack(j)) {
        const auto event = j.find("e");
        if (event == j.end() || !event->is_string()) {
            return;
        }

        std::string senderString;
        const auto sender = j.find("s");
        if (!(sender == j.end() || !sender->is_string())) {
            senderString = sender->get<std::string>();
        }

        const auto json = j.find("j");
        if (json == j.end() || !json->is_object()) {
            return;
        }

        const auto eventString = event->get<std::string>();

        ProcessNetMessage(eventString, json.value(), senderString);
    }
}

void jleNetScene::OnSceneCreation() {
    _networking.Connect(
        _connectAddress,
        [&]() { OnNetConnected(); },
        [&](int const& reason) {
            if (reason == 1) {
                OnNetClosed("drop");
            }
            else {
                OnNetClosed("normal");
            }
        },
        [&]() { OnNetFailed(); });
}

void jleNetScene::OnSceneDestruction() { _networking.Disconnect(); }

void jleNetScene::ProcessNetMessage(const std::string& event,
                                    const nlohmann::json& message,
                                    const std::string& sender) {
    if (event == "you_r_host") {
        _isHost = true;
        return;
    }

    if (event == "SceneSync") {
        _sceneObjects.clear();
        _newSceneObjects.clear();

        jleScene::FromJson(message);
        return;
    }

    if (event == "player_connect" && _isHost) {
        nlohmann::json j;
        ToJson(j);
        _networking.EmitJsonData("SceneSync", j, sender);
    }

    if (event == "player_dconnect") {
        const std::string playerId = message["id"];
        LOGV << "player dconnect: " << playerId;
    }
}
