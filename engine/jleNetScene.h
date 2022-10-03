// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleNetworking.h"
#include "jleScene.h"

class jleNetScene : public jleScene {
public:
    jleNetScene();

    explicit jleNetScene(const std::string& sceneName);

    void SceneUpdate() override;

    void OnSceneCreation() override;

    void OnSceneDestruction() override;

    virtual void ProcessNetMessage(const std::string& event,
                                   const nlohmann::json& message,
                                   const std::string& sender);

    virtual void OnNetConnected(){};

    virtual void OnNetFailed(){};

    virtual void OnNetClosed(const std::string& reason){};

    void ToJson(nlohmann::json& j_out) override;

    void FromJson(const nlohmann::json& j_in) override;

protected:
    jleNetworking _networking;

    // TODO: Move this to more flexible solution
#ifdef __EMSCRIPTEN__
    std::string _connectAddress{"hexablo.herokuapp.com"};
#else
    // std::string _connectAddress{"http://localhost:314"};
    std::string _connectAddress{"http://hexablo.herokuapp.com"};
#endif

    bool _isHost = false;
};
