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

#include "jleDefaultGameFunctions.h"

#include "jleGameEngine.h"

#include "modules/graphics/core/jleFrameBufferInterface.h"
#include "modules/graphics/jleGraphicsModule.h"
#include "modules/hierarchy/jleHierarchyModule.h"
#include "modules/input/jleInputModule.h"
#include "modules/jleEngineUpdateContext.h"
#include "modules/mesh/jleMeshModule.h"
#include "modules/physics/jlePhysicsModule.h"
#include "modules/scripting/jleLuaModule.h"
#include "modules/windowing/jleWindowModule.h"

std::unique_ptr<jleGameModules>
jleDefaultGameFunctions::createDefaultModules(bool gameRunning)
{
    auto modules = std::make_unique<jleGameModules>();

    modules->addModule<jleGameWindowModule, jleWindowModuleBase>(std::make_unique<jleGameWindowModule>());
    modules->addModule<jleGameInputModule, jleInputModuleBase>(std::make_unique<jleGameInputModule>(modules->getModule<jleGameWindowModule>()->getWindow()));
    modules->addModule<jleHierarchyModule>(std::make_unique<jleHierarchyModule>());
    modules->addModule<jleMeshModule>(std::make_unique<jleMeshModule>());
    modules->addModule<jleGraphicsModule>(std::make_unique<jleGraphicsModule>());
    modules->addModule<jlePhysicsModule>(std::make_unique<jlePhysicsModule>());
    modules->addModule<jleLuaModule>(std::make_unique<jleLuaModule>());

    return modules;
}

void
jleDefaultGameFunctions::defaultModulesInitialize(jleGameModules &modules,
                                                  jlECS::ECS &ecs,
                                                  jleSerializationContext &serializationContext)
{
    auto* windowModule = modules.getModule<jleWindowModuleBase>();
    auto *hierarchyModule = modules.getModule<jleHierarchyModule>();
    auto *meshModule = modules.getModule<jleMeshModule>();
    auto *graphicsModule = modules.getModule<jleGraphicsModule>();
    auto *physicsModule = modules.getModule<jlePhysicsModule>();
    auto *luaModule = modules.getModule<jleLuaModule>();

    assert(hierarchyModule && meshModule && graphicsModule && physicsModule && luaModule);

    if (windowModule)
        windowModule->initWindowModule();

    hierarchyModule->initializeECS(ecs);
    meshModule->initializeECS(ecs);

    graphicsModule->initializeModule(serializationContext);
    graphicsModule->initializeECS(ecs);

    physicsModule->initializeECS(ecs);

    luaModule->initializeModule(serializationContext);
    luaModule->initializeECS(ecs);
}


void jleDefaultGameFunctions::defaultModulesUpdate(jleGameModules& modules, jleEngineUpdateContext& ctx, jlECS::ECS& ecs) {
    auto* windowModule = modules.getModule<jleWindowModuleBase>();
    auto* inputModule = modules.getModule<jleInputModuleBase>();
    auto* hierarchyModule = modules.getModule<jleHierarchyModule>();
    auto* meshModule = modules.getModule<jleMeshModule>();
    auto* graphicsModule = modules.getModule<jleGraphicsModule>();
    auto* physicsModule = modules.getModule<jlePhysicsModule>();
    auto* luaModule = modules.getModule<jleLuaModule>();

    assert(meshModule && inputModule && hierarchyModule && physicsModule && luaModule);

    inputModule->update();

    meshModule->loadMeshes(ecs);

    hierarchyModule->updateWorldMatrices(ecs);
    const std::vector<glm::mat4>& worldMatrices = hierarchyModule->getWorldMatrices();

    if (windowModule) {
        uint32_t windowX = windowModule->getWindowWidth();
        uint32_t windowY = windowModule->getWindowHeight();

        if (graphicsModule) {
            jleGraphicsModule::UpdateContext graphicsUpdateContext{
                .in =    {   .screenX = windowX,
                                .screenY = windowY,
                                .worldMatrices = worldMatrices,
                                .meshModule = meshModule },
                .inOut = {  .ecs = ecs }
            };

            graphicsModule->update(graphicsUpdateContext);
        }
    }

    {
        jlePhysicsModule::UpdateContext physicsUpdateContext{
            .in =     { .dt = 1.f / 60.f,
                        .worldMatrices = worldMatrices },
            .inOut =  { .ecs = ecs}
        };

        physicsModule->update(physicsUpdateContext);
    }

    {
        jleLuaModule::UpdateContext luaUpdateContext{
            .in =    { .dt = ctx.frameInfo.getDeltaTime() },
            .inOut = { .ecs = ecs }
        };

        luaModule->update(luaUpdateContext);
    }
}

void
jleDefaultGameFunctions::defaultModulesUpdateRenderablesOnly(jleGameModules &modules,
                                                             jleEngineUpdateContext &ctx,
                                                             jlECS::ECS &ecs)
{
    auto *hierarchyModule = modules.getModule<jleHierarchyModule>();
    auto *graphicsModule = modules.getModule<jleGraphicsModule>();
    auto *meshModule = modules.getModule<jleMeshModule>();

    assert(meshModule && hierarchyModule && graphicsModule);

    hierarchyModule->updateWorldMatrices(ecs);
    const std::vector<glm::mat4> &worldMatrices = hierarchyModule->getWorldMatrices();

    meshModule->loadMeshes(ecs);

    {
        jleGraphicsModule::UpdateContext graphicsUpdateContext{
            .in = {     .screenX = 1920,
                        .screenY = 1080,
                        .worldMatrices = worldMatrices,
                        .meshModule = meshModule},
            .inOut = {.ecs = ecs}
        };

        graphicsModule->update(graphicsUpdateContext);
    }
}

void
jleDefaultGameFunctions::defaultPopulateSerializeableInterfaces(jleGameModules &modules,
                                                                std::vector<jleSerializableInterface *> &interfaces)
{
    auto *graphicsModule = modules.getModule<jleGraphicsModule>();
    assert(graphicsModule);
    graphicsModule->populateSerializeableInterface(interfaces);

    auto *luaModule = modules.getModule<jleLuaModule>();
    assert(luaModule);
    luaModule->populateSerializeableInterface(interfaces);
}

void
jleDefaultGameFunctions::defaultPreRender(jleGameModules &modules, jleSerializationContext &ctx)
{
    if (auto *graphicsModule = modules.getModule<jleGraphicsModule>()) {
        graphicsModule->preRender();
    }
}

bool
jleDefaultGameFunctions::defaultRender(jleGameModules &modules, jleSerializationContext &ctx)
{
    if (auto *windowModule = modules.getModule<jleWindowModuleBase>()) {
        if (auto *graphicsModule = modules.getModule<jleGraphicsModule>()) {
            graphicsModule->render(windowModule->getWindowWidth(), windowModule->getWindowHeight());
        }

        windowModule->updateWindowModule();
        return windowModule->windowShouldClose();
    }
    return false;
}

void
jleDefaultGameFunctions::defaultPostRender(jleGameModules &modules, jleSerializationContext &ctx)
{
    if (auto *graphicsModule = modules.getModule<jleGraphicsModule>()) {
        graphicsModule->postRender();
    }
}