// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleNetworking.h"
#include "jleScene.h"

class jleNetScene : public jleScene {
public:
    jleNetScene();

    explicit jleNetScene(const std::string &sceneName);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(cereal::base_class<jleScene>(this));
    }

    void updateScene() override;

    void onSceneCreation() override;

    void onSceneDestruction() override;

    virtual void processNetMessage(const std::string &event,
                                   const nlohmann::json &message,
                                   const std::string &sender);

    virtual void onNetConnected(){};

    virtual void onNetFailed(){};

    virtual void onNetClosed(const std::string &reason){};

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

CEREAL_REGISTER_TYPE(jleNetScene)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleScene, jleNetScene)