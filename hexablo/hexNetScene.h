// Copyright (c) 2022. Johan Lind

#ifndef HEXABLO_HEXNETSCENE_H
#define HEXABLO_HEXNETSCENE_H

#include "jleNetScene.h"

class hexNetScene : public jle::jleNetScene {
public:
    void SceneUpdate() override;

    void OnNetConnected() override;

    void OnNetFailed() override;

    void OnNetClosed(const std::string &reason) override;

    void ProcessNetMessage(const std::string &event, const nlohmann::json &message, const std::string &sender) override;

    std::map<std::string, std::weak_ptr<jle::jleObject>> mPlayers;

};


#endif //HEXABLO_HEXNETSCENE_H
