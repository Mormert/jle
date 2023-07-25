// Copyright (c) 2023. Johan Lind

#pragma once

#include <memory>
#include <vector>

#include "jleCamera.h"
#include "jleCore.h"
#include "jleProfiler.h"
#include "jleResource.h"
#include "jleScene.h"

#include <cereal/archives/json.hpp>

#ifdef BUILD_EDITOR
#include "editor/jleImGuiCerealArchive.h"
#endif

#include <fstream>
#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include <sol2/sol.hpp>

class jleGame
{
public:
    jleGame();

    virtual ~jleGame() = default;

    virtual void
    update(float dt)
    {
    }

    virtual void
    start()
    {
    }

    void updateActiveScenes(float dt);

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

    std::shared_ptr<jleScene>
    loadScene(const jlePath &scenePath)
    {
        std::shared_ptr<jleScene> scene = gCore->resources().loadResourceFromFile<jleScene>(scenePath, true);
        if (scene) {
            auto it = std::find(_activeScenes.begin(), _activeScenes.end(), scene);
            if (it == _activeScenes.end()) {
                _activeScenes.push_back(scene);
                scene->onSceneCreation();
                scene->startObjects();
            } else {
                LOG_WARNING << "Loaded scene is already loaded";
            }
        }

        return scene;
    }

    std::vector<std::shared_ptr<jleScene>> &activeScenesRef();

    jleCamera mainCamera{jleCameraProjection::Orthographic};


protected:
    std::vector<std::shared_ptr<jleScene>> _activeScenes;
};