// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleNetScene.h"
#include "oFireball.h"

class hexNetScene : public jleNetScene {
public:
    void sceneupdate() override;

    void onNetConnected() override;

    void onNetFailed() override;

    void onNetClosed(const std::string &reason) override;

    void processNetMessage(const std::string &event,
                           const nlohmann::json &message,
                           const std::string &sender) override;

private:
    std::weak_ptr<jleObject> playerFromId(const std::string &id);

    std::map<std::string, std::weak_ptr<jleObject>> _players;

    std::weak_ptr<oFireball> fireballFromId(const int id);
    std::map<int, std::weak_ptr<oFireball>> _fireballs;
};
