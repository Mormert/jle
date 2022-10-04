// Copyright (c) 2022. Johan Lind

#include "hexNetScene.h"
#include "oCharacter.h"

void hexNetScene::onNetConnected() { std::cout << "Net connected\n"; }

void hexNetScene::onNetFailed() { std::cout << "Net failure\n"; }

void hexNetScene::onNetClosed(const std::string& reason) {
    std::cout << "Net closed: " << reason << '\n';
}

void hexNetScene::sceneUpdate() { jleNetScene::sceneUpdate(); }

void hexNetScene::processNetMessage(const std::string& event,
                                    const nlohmann::json& message,
                                    const std::string& sender) {
    jleNetScene::processNetMessage(event, message, sender);

    if (event == "player_connect") {
        const std::string id = message["id"];
        playerFromId(id); // will also create the player
    }

    if (event == "oFireball") {
        const int id = message["id"];
        auto fb = fireballFromId(id).lock();
        fb->fromNet(message);
    }

    if (event == "player_pos") {
        if (_players.find(sender) == _players.end()) {
            std::weak_ptr<jleObject> newPlayer = spawnTemplateObject(
                jleRelativePath{"GR:/otemps/otherPlayer.tmpl"});
            _players[sender] = newPlayer;
        }

        if (auto otherPlayer = _players[sender].lock()) {
            int q = message["q"];
            int r = message["r"];
            int d = message["d"];
            std::static_pointer_cast<oCharacter>(otherPlayer)
                ->hexagonPlacementInterp(q, r);
            std::static_pointer_cast<oCharacter>(otherPlayer)
                ->characterDirection(
                    static_cast<oCharacter::oCharacterDirection>(d));
        }
        else {
            _players.erase(sender);
        }

        return;
    }

    if (event == "basic_attack") {
        auto player = playerFromId(sender).lock();
        int q = message["q"];
        int r = message["r"];
        int d = message["d"];
        std::static_pointer_cast<oCharacter>(player)
            ->hexagonPlacementTeleport(q, r);
        std::static_pointer_cast<oCharacter>(player)->attack(
            static_cast<oCharacter::oCharacterDirection>(d));
    }

    if (event == "player_dconnect") {
        const std::string id = message["id"];
        auto player = playerFromId(id).lock();
        player->destroyObject();
    }
}

std::weak_ptr<jleObject> hexNetScene::playerFromId(const std::string& id) {
    auto it = _players.find(id);
    if (it == _players.end() ||
        (it != _players.end() && it->second.expired())) {
        std::weak_ptr<jleObject> newPlayer =
            spawnTemplateObject(jleRelativePath{"GR:/otemps/otherPlayer.tmpl"});
        _players[id] = newPlayer;
        return newPlayer;
    }
    return it->second.lock();
}

std::weak_ptr<oFireball> hexNetScene::fireballFromId(const int id) {
    auto it = _fireballs.find(id);
    if (it == _fireballs.end() ||
        (it != _fireballs.end() && it->second.expired())) {
        auto newFireball =
            std::static_pointer_cast<oFireball>(spawnTemplateObject(
                jleRelativePath{"GR:otemps/FireballTempl.tmpl"}));
        _fireballs[id] = newFireball;
        return newFireball;
    }
    return it->second.lock();
}
