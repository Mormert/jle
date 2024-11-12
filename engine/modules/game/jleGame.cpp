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

#include "core/jlECS/jlECS.h"
#include "modules/graphics/core/jleFrameBufferInterface.h"
#include <modules/graphics/jleRenderThread.h>
#include "modules/physics/jlePhysics.h"

#include <3rdparty/WickedEngine/wiJobSystem.h>

void
jleGame::updateActiveScenes(jleEngineUpdateContext &ctx)
{
    JLE_SCOPE_PROFILE_CPU(jleGame_updateActiveScenes)
    for (int i = _activeScenes.size() - 1; i >= 0; i--) {
        if (_activeScenes[i]->bPendingSceneDestruction) {
            _activeScenes.erase(_activeScenes.begin() + i);
            continue;
        }

        _activeScenes[i]->updateScene(ctx);
    }
}

std::vector<std::shared_ptr<jleScene>> &
jleGame::activeScenesRef()
{
    return _activeScenes;
}

std::shared_ptr<jleScene>
jleGame::loadScene(const jlePath &scenePath, jleEngineUpdateContext &ctx)
{
    // jleSerializationContext serializationContext{&ctx.resourcesModule, &ctx.luaEnvironment, &ctx.renderThread};

    std::shared_ptr<jleScene> scene =
        ctx.resourcesModule.loadResourceFromFileT<jleScene>(scenePath, ctx.serializationContext, true);
    if (scene) {
        auto it = std::find(_activeScenes.begin(), _activeScenes.end(), scene);
        if (it == _activeScenes.end()) {
            _activeScenes.push_back(scene);
            scene->onSceneStart(ctx);
            scene->startObjects(ctx);
        } else {
            LOG_WARNING << "Loaded scene is already loaded";
        }
    }

    return scene;
}

jleGame::jleGame() { _gameState._ecs = std::make_unique<jlECS::ECS>(); }

jleGame::~jleGame() = default;

void
jleGame::parallelUpdates(jleEngineUpdateContext &ctx)
{
    ZoneScoped;
    wi::jobsystem::context parallelUpdatesCtx;

    for (const auto &parallelizedComponents : _parallelComponents) {

        const auto &components = parallelizedComponents.second;

        if (!components.empty()) {

            int batchSize = components[0]->parallelUpdateBatchSize();
            wi::jobsystem::Dispatch(parallelUpdatesCtx, components.size(), batchSize, [&](wi::jobsystem::JobArgs args) {
                ZoneScopedNC("ParallelUpdate", 0xFF8200);
                const int componentIdx = args.jobIndex;
                auto &component = components[componentIdx];
                component->parallelUpdate(ctx);
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

void
jleGame::update(jleEngineUpdateContext &ctx)
{
    jleGraphicsModule::UpdateContext graphicsUpdateContext{
        .in = {.screenX = static_cast<int>(ctx.gameRuntime.mainGameScreenFramebuffer->width()),
               .screenY = static_cast<int>(ctx.gameRuntime.mainGameScreenFramebuffer->height())},
        .inOut = {.ecs = *_gameState._ecs},
        .out = {.framePacket = ctx.currentFramePacket, .camera = mainCamera}};

    // _graphicsModule.update(graphicsUpdateContext);
}

void
jleGame::start(jleEngineUpdateContext &ctx)
{
    _graphicsModule.initializeECS(*_gameState._ecs);
    _physicsModule.initializeECS(*_gameState._ecs);
}
