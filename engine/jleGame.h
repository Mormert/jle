// Copyright (c) 2023. Johan Lind

#pragma once

#include <memory>
#include <vector>

#include "jleCamera.h"
#include "jleGameEngine.h"
#include "jleProfiler.h"
#include "jleResource.h"
#include "jleScene.h"

#include <cereal/archives/json.hpp>

#ifdef BUILD_EDITOR
#include "editor/jleImGuiCerealArchive.h"
#endif

#include <execution>
#include <fstream>
#include <iostream>
#include <typeinfo>

#include <3rdparty/WickedEngine/wiJobSystem.h>

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
        std::shared_ptr<jleScene> scene = gEngine->resources().loadResourceFromFile<jleScene>(scenePath, true);
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

    void
    parallelUpdates(float dt)
    {
        wi::jobsystem::context parallelUpdatesCtx;
        for (const auto &parallelizedComponents : _parallelComponents) {
            for (const auto component : parallelizedComponents.second) {
                wi::jobsystem::Execute(parallelUpdatesCtx, [component, dt](wi::jobsystem::JobArgs args) {
                    component->parallelUpdate(dt);
                });
            }

             wi::jobsystem::Wait(parallelUpdatesCtx);
        }
    }

    void
    addParallelComponent(const std::shared_ptr<jleComponent> &component)
    {
        const auto type = typeid(component).hash_code();
        _parallelComponents[type].push_back(component);
    }

protected:
    std::vector<std::shared_ptr<jleScene>> _activeScenes;

    std::unordered_map<uint64_t, std::vector<std::shared_ptr<jleComponent>>> _parallelComponents;
};