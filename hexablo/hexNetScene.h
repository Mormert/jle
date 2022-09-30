// Copyright (c) 2022. Johan Lind

#ifndef HEXABLO_HEXNETSCENE_H
#define HEXABLO_HEXNETSCENE_H

#include "jleNetScene.h"
#include "oFireball.h"

class hexNetScene : public jleNetScene {
public:
    void SceneUpdate() override;

    void OnNetConnected() override;

    void OnNetFailed() override;

    void OnNetClosed(const std::string& reason) override;

    void ProcessNetMessage(const std::string& event,
                           const nlohmann::json& message,
                           const std::string& sender) override;

private:
    std::weak_ptr<jleObject> GetPlayerFromId(const std::string& id);

    std::map<std::string, std::weak_ptr<jleObject>> _players;

    std::weak_ptr<oFireball> GetFireballFromId(const int id);
    std::map<int, std::weak_ptr<oFireball>> _fireballs;
};

#endif // HEXABLO_HEXNETSCENE_H
