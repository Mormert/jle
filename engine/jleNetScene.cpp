// Copyright (c) 2023. Johan Lind

#include "jleNetScene.h"
#include "jleCore.h"
#include "plog/Log.h"

jleNetScene::jleNetScene() : jleScene() {}

jleNetScene::jleNetScene(const std::string &sceneName) : jleScene(sceneName) {}

void jleNetScene::toJson(nlohmann::json &j_out) {
    jleScene::toJson(j_out);
    j_out["_connectAddress"] = _connectAddress;

    if (_isHost) {
        _networking.emitJsonData("SceneSync", j_out);
    }
}

void jleNetScene::fromJson(const nlohmann::json &j_in) {
    jleScene::fromJson(j_in);
    JLE_FROM_JSON_IF_EXISTS(j_in, _connectAddress, "_connectAddress");
}

void jleNetScene::updateScene() {
    nlohmann::json j;
    // Sync data back to game thread, and process the network message
    if (_networking.tryReceiveMessagePack(j)) {
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

        processNetMessage(eventString, json.value(), senderString);
    }
}

void jleNetScene::onSceneCreation() {
    _networking.connect(
        _connectAddress,
        [&]() { onNetConnected(); },
        [&](int const &reason) {
            if (reason == 1) {
                onNetClosed("drop");
            }
            else {
                onNetClosed("normal");
            }
        },
        [&]() { onNetFailed(); });
}

void jleNetScene::onSceneDestruction() { _networking.disconnect(); }

void jleNetScene::processNetMessage(const std::string &event,
                                    const nlohmann::json &message,
                                    const std::string &sender) {
    if (event == "you_r_host") {
        _isHost = true;
        return;
    }

    if (event == "SceneSync") {
        _sceneObjects.clear();
        _newSceneObjects.clear();

        jleScene::fromJson(message);
        return;
    }

    if (event == "player_connect" && _isHost) {
        nlohmann::json j;
        toJson(j);
        _networking.emitJsonData("SceneSync", j, sender);
    }

    if (event == "player_dconnect") {
        const std::string playerId = message["id"];
        LOGV << "player dconnect: " << playerId;
    }
}
