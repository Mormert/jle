// Copyright (c) 2022. Johan Lind

#include "hexNetScene.h"
#include "oCharacter.h"

void hexNetScene::OnNetConnected() {
    std::cout << "Net connected\n";
}

void hexNetScene::OnNetFailed() {
    std::cout << "Net failure\n";
}

void hexNetScene::OnNetClosed(const std::string &reason) {
    std::cout << "Net closed: " << reason << '\n';
}

void hexNetScene::SceneUpdate() {
    jleNetScene::SceneUpdate();
}

void
hexNetScene::ProcessNetMessage(const std::string &event, const nlohmann::json &message, const std::string &sender) {
    jleNetScene::ProcessNetMessage(event, message, sender);

    if (event == "player_pos") {
        if (mPlayers.find(sender) == mPlayers.end()) {
            std::weak_ptr<jle::jleObject> newPlayer = SpawnTemplateObject("GR:/otemps/otherPlayer.tmpl");
            mPlayers[sender] = newPlayer;
        }

        if (auto otherPlayer = mPlayers[sender].lock()) {
            int q = message["q"];
            int r = message["r"];
            int d = message["d"];
            std::static_pointer_cast<oCharacter>(otherPlayer)->SetHexagonPlacementInterp(q, r);
            std::static_pointer_cast<oCharacter>(otherPlayer)->SetCharacterDirection(
                    static_cast<oCharacter::oCharacterDirection>(d));
        } else {
            mPlayers.erase(sender);
        }

        return;
    }
}
