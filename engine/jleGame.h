// Copyright (c) 2023. Johan Lind

#pragma once

#include <memory>
#include <vector>

#include "editor/jleEditorSettings.h"
#include "jleCamera.h"
#include "jleGameSettings.h"
#include "jleProfiler.h"
#include "jleScene.h"

#include <cereal/archives/json.hpp>
#include "editor/jleImGuiCerealArchive.h"


#include <fstream>
#include <iostream>

class jleGame
{
public:
    virtual ~jleGame() = default;

    virtual void
    update(float dt)
    {
    }

    virtual void
    start()
    {
    }

    static void
    overrideGameSettings(jleGameSettings &gs)
    {
    }

    static void
    overrideGameEditorSettings(jleGameSettings &gs, jleEditorSettings &es)
    {
    }

    void updateActiveScenes(float dt);

    bool checkSceneIsActive(const std::string &sceneName);

    template <typename T>
    std::shared_ptr<T>
    createScene()
    {
        static_assert(std::is_base_of<jleScene, T>::value, "T must derive from jleScene");

        std::shared_ptr<T> newScene = std::make_shared<T>();
        _activeScenes.push_back(newScene);

        newScene->onSceneCreation();

        return newScene;
    }

    template <typename T>
    std::shared_ptr<jleScene>
    loadScene(const std::string &scenePath)
    {
        static_assert(std::is_base_of<jleScene, T>::value, "T must derive from jleScene");

        if (checkSceneIsActive(scenePath)) {
            LOG_WARNING << "Loaded scene is already loaded";
            return nullptr;
        }

        std::ifstream i(scenePath);
        if (i.good()) {
            std::shared_ptr<T> scene = createScene<T>();

            try {
                std::ifstream ix(scenePath);
                cereal::JSONInputArchive iarchive{ix};
                scene->serialize(iarchive);
                scene->startObjects();
            } catch (std::exception &e) {
                LOG_ERROR << "Could not load scene with path: " << scenePath << ", err:\n" << e.what();
                return nullptr;
            }

            return scene;
        } else {
            LOG_ERROR << "Could not load scene with path: " << scenePath;
            return nullptr;
        }
    }

    std::vector<std::shared_ptr<jleScene>> &activeScenesRef();

    jleCamera mainCamera{jleCameraProjection::Orthographic};

protected:
    std::vector<std::shared_ptr<jleScene>> _activeScenes;
};