// Copyright (c) 2022. Johan Lind


#ifndef JLENETSCENE_H
#define JLENETSCENE_H

#include "jleScene.h"
#include "jleNetworking.h"

namespace jle {
    class jleNetScene : public jleScene {
    public:
        jleNetScene();

        explicit jleNetScene(const std::string &sceneName);

        void SceneUpdate() override;

        void OnSceneCreation() override;

        void OnSceneDestruction() override;

        virtual void
        ProcessNetMessage(const std::string &event, const nlohmann::json &message, const std::string &sender);

        virtual void OnNetConnected() {};

        virtual void OnNetFailed() {};

        virtual void OnNetClosed(const std::string &reason) {};

        void ToJson(nlohmann::json &j_out) override;

        void FromJson(const nlohmann::json &j_in) override;

    protected:
        jleNetworking mNetworking;

        // TODO: Move this to more flexible solution
#ifdef __EMSCRIPTEN__
        std::string mConnectAddress{"localhost:314"};
#else
        std::string mConnectAddress{"http://localhost:314"};
#endif

        bool mIsHost = false;
    };

}


#endif // JLENETSCENE_H
