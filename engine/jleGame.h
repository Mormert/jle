/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#pragma once

#include <memory>
#include <vector>

#include "jleCamera.h"
#include "jleGameEngine.h"
#include "jleProfiler.h"
#include "jleResource.h"
#include "jleScene.h"

#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

#ifdef JLE_BUILD_EDITOR
#include "editor/jleImGuiCerealArchive.h"
#endif

#include <execution>
#include <fstream>
#include <iostream>
#include <typeinfo>

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

        newScene->onSceneStart();

        return newScene;
    }

    std::shared_ptr<jleScene> loadScene(const jlePath &scenePath);

    std::vector<std::shared_ptr<jleScene>> &activeScenesRef();

    jleCamera mainCamera{jleCameraProjection::Orthographic};

    void parallelUpdates(float dt);

    void addParallelComponent(const std::shared_ptr<jleComponent> &component);

    void removeParallelComponent(const std::shared_ptr<jleComponent> &component);

protected:
    std::vector<std::shared_ptr<jleScene>> _activeScenes;

    std::unordered_map<uint64_t, std::vector<std::shared_ptr<jleComponent>>> _parallelComponents;
};