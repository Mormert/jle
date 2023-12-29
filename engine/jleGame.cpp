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

#include "jleGame.h"
#include <glm/ext/matrix_transform.hpp>

#include <3rdparty/WickedEngine/wiJobSystem.h>


void
jleGame::updateActiveScenes(float dt)
{
    JLE_SCOPE_PROFILE_CPU(jleGame_updateActiveScenes)
    for (int i = _activeScenes.size() - 1; i >= 0; i--) {
        if (_activeScenes[i]->bPendingSceneDestruction) {
            _activeScenes.erase(_activeScenes.begin() + i);
            continue;
        }

        _activeScenes[i]->updateScene(dt);
    }
}

std::vector<std::shared_ptr<jleScene>> &
jleGame::activeScenesRef()
{
    return _activeScenes;
}

std::shared_ptr<jleScene>
jleGame::loadScene(const jlePath &scenePath)
{
    std::shared_ptr<jleScene> scene = gEngine->resources().loadResourceFromFile<jleScene>(scenePath, true);
    if (scene) {
        auto it = std::find(_activeScenes.begin(), _activeScenes.end(), scene);
        if (it == _activeScenes.end()) {
            _activeScenes.push_back(scene);
            scene->onSceneStart();
            scene->startObjects();
        } else {
            LOG_WARNING << "Loaded scene is already loaded";
        }
    }

    return scene;
}

jleGame::jleGame() {}

void
jleGame::parallelUpdates(float dt)
{
    ZoneScoped;
    wi::jobsystem::context parallelUpdatesCtx;

    for (const auto &parallelizedComponents : _parallelComponents) {

        const auto &components = parallelizedComponents.second;

        if (!components.empty()) {

            int batchSize = components[0]->parallelUpdateBatchSize();
            wi::jobsystem::Dispatch(
                parallelUpdatesCtx, components.size(), batchSize, [components, dt](wi::jobsystem::JobArgs args) {
                    ZoneScopedNC("ParallelUpdate", 0xFF8200);
                    const int componentIdx = args.jobIndex;
                    auto &component = components[componentIdx];
                    component->parallelUpdate(dt);
                });
        }

        {
            ZoneScopedN("Wait For Jobs");
            wi::jobsystem::Wait(parallelUpdatesCtx);
        }
    }
}

void
jleGame::addParallelComponent(const std::shared_ptr<jleComponent> &component)
{
    const auto type = typeid(component).hash_code();
    _parallelComponents[type].push_back(component);
}

void
jleGame::removeParallelComponent(const std::shared_ptr<jleComponent> &component)
{
    const auto type = typeid(component).hash_code();
    auto &vec = _parallelComponents[type];
    if (!vec.empty()) {
        vec.erase(std::remove(vec.begin(), vec.end(), component), vec.end());
    }
}
