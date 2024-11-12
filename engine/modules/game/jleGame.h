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

#include "core/jleCommon.h"

#include <memory>
#include <vector>

#include "core/jleCamera.h"
#include "core/jleProfiler.h"
#include "core/jleResourceHolder.h"
#include "core/jleScene.h"
#include "jleGameEngine.h"

#include <modules/graphics/jleGraphicsModule.h>
#include <modules/physics/jlePhysicsModule.h>

#include <execution>
#include <fstream>
#include <iostream>
#include <typeinfo>

namespace jlECS
{
class ECS;
}

class jleGame
{
public:
    jleGame();

    virtual ~jleGame();

    virtual void update(jleEngineUpdateContext &ctx);

    virtual void
    start(jleEngineUpdateContext &ctx);

    void updateActiveScenes(jleEngineUpdateContext &ctx);

    template <typename T>
    std::shared_ptr<T>
    createScene(jleEngineUpdateContext &ctx)
    {
        static_assert(std::is_base_of<jleScene, T>::value, "T must derive from jleScene");

        std::shared_ptr<T> newScene = std::make_shared<T>();
        _activeScenes.push_back(newScene);

        newScene->onSceneStart(ctx);

        return newScene;
    }

    std::shared_ptr<jleScene> loadScene(const jlePath &scenePath, jleEngineUpdateContext &ctx);

    std::vector<std::shared_ptr<jleScene>> &activeScenesRef();

    jleCamera mainCamera{jleCameraProjection::Orthographic};

    void parallelUpdates(jleEngineUpdateContext &ctx);

    void addParallelComponent(const std::shared_ptr<jleComponent> &component);

    void removeParallelComponent(const std::shared_ptr<jleComponent> &component);

    jlECS::ECS &
    getECS()
    {
        return *_gameState._ecs;
    }

protected:
    std::vector<std::shared_ptr<jleScene>> _activeScenes;

    jleGraphicsModule _graphicsModule;
    jlePhysicsModule _physicsModule;

    struct GameState
    {
        std::unique_ptr<jlECS::ECS> _ecs;
        std::unique_ptr<jlePhysics> _physics;
    } _gameState;


    std::unordered_map<uint64_t, std::vector<std::shared_ptr<jleComponent>>> _parallelComponents;
};