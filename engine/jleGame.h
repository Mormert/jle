// Copyright (c) 2022. Johan Lind

#pragma once

#include <memory>
#include <vector>

#include "editor/jleEditorSettings.h"
#include "jleCamera.h"
#include "jleGameSettings.h"
#include "jleProfiler.h"
#include "jleScene.h"

#include <fstream>
#include <iostream>

class jleGame {
public:
    virtual ~jleGame() = default;

    virtual void Update(float dt) {}

    virtual void Start() {}

    static void OverrideGameSettings(jleGameSettings& gs) {}

    static void OverrideGameEditorSettings(jleGameSettings& gs,
                                           jleEditorSettings& es) {}

    void UpdateActiveScenes(float dt);

    bool CheckSceneIsActive(const std::string& sceneName);

    template <typename T>
    std::shared_ptr<T> CreateScene() {
        static_assert(std::is_base_of<jleScene, T>::value,
                      "T must derive from jleScene");

        std::shared_ptr<T> newScene = std::make_shared<T>();
        _activeScenes.push_back(newScene);

        newScene->OnSceneCreation();

        return newScene;
    }

    template <typename T>
    std::shared_ptr<jleScene> LoadScene(const std::string& scenePath) {

        static_assert(std::is_base_of<jleScene, T>::value,
                      "T must derive from jleScene");

        if (CheckSceneIsActive(scenePath)) {
            LOG_WARNING << "Loaded scene is already loaded";
            return nullptr;
        }

        std::ifstream i(scenePath);
        if (i.good()) {
            std::shared_ptr<T> scene = CreateScene<T>();
            nlohmann::json j;
            i >> j;

            from_json(j, *scene);
            return scene;
        }
        else {
            LOG_ERROR << "Could not load scene with path: " << scenePath;
            return nullptr;
        }
    }

    std::vector<std::shared_ptr<jleScene>>& GetActiveScenesRef();

    jleCamera _mainCamera{jleCameraProjection::Orthographic};

protected:
    std::vector<std::shared_ptr<jleScene>> _activeScenes;
};