// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleNetworking.h"
#include "jleScene.h"

class jleNetScene : public jleScene {
public:
    jleNetScene();

    explicit jleNetScene(const std::string& sceneName);

    void sceneUpdate() override;

    void onSceneCreation() override;

    void onSceneDestruction() override;

    virtual void processNetMessage(const std::string& event,
                                   const nlohmann::json& message,
                                   const std::string& sender);

    virtual void onNetConnected(){};

    virtual void onNetFailed(){};

    virtual void onNetClosed(const std::string& reason){};

    void toJson(nlohmann::json& j_out) override;

    void fromJson(const nlohmann::json& j_in) override;

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
