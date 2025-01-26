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
#include "jleGameRuntime.h"

#include "modules/graphics/core/jleFrameBufferInterface.h"
#include "modules/graphics/jleGraphicsModule.h"
#include "modules/hierarchy/jleHierarchyModule.h"
#include "modules/input/jleInputModule.h"
#include "modules/jleEngineUpdateContext.h"
#include "modules/physics/jlePhysicsModule.h"
#include "modules/scripting/jleLuaModule.h"
#include "modules/windowing/jleWindowModule.h"

std::unique_ptr<jleGameModules>
jleDefaultGameFunctions::createDefaultModules(bool gameRunning)
{
    auto modules = std::make_unique<jleGameModules>();

    modules->addModule<jleWindowModule>(std::make_unique<jleWindowModule>());
    modules->addModule<jleInputModule>(std::make_unique<jleInputModule>(modules->getModule<jleWindowModule>()->window));
    modules->addModule<jleHierarchyModule>(std::make_unique<jleHierarchyModule>());
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
    auto* windowModule = modules.getModule<jleWindowModule>();
    auto *hierarchyModule = modules.getModule<jleHierarchyModule>();
    auto *graphicsModule = modules.getModule<jleGraphicsModule>();
    auto *physicsModule = modules.getModule<jlePhysicsModule>();
    auto *luaModule = modules.getModule<jleLuaModule>();

    assert(hierarchyModule && graphicsModule && physicsModule && luaModule);

    if (windowModule)
        windowModule->initWindowModule();

    hierarchyModule->initializeECS(ecs);

    graphicsModule->initializeModule(serializationContext);
    graphicsModule->initializeECS(ecs);

    physicsModule->initializeECS(ecs);

    luaModule->initializeModule(serializationContext);
    luaModule->initializeECS(ecs);
}


void jleDefaultGameFunctions::defaultModulesUpdate(jleGameModules& modules, jleEngineUpdateContext& ctx, jlECS::ECS& ecs) {
    auto* windowModule = modules.getModule<jleWindowModule>();
    auto* inputModule = modules.getModule<jleInputModule>();
    auto* hierarchyModule = modules.getModule<jleHierarchyModule>();
    auto* graphicsModule = modules.getModule<jleGraphicsModule>();
    auto* physicsModule = modules.getModule<jlePhysicsModule>();
    auto* luaModule = modules.getModule<jleLuaModule>();

    assert(windowModule && inputModule && hierarchyModule && graphicsModule && physicsModule && luaModule);

    auto windowDimensions = windowModule->window.getDimensions();
    uint32_t windowX = windowDimensions.framebufferWidth;
    uint32_t windowY = windowDimensions.framebufferHeight;

    inputModule->mouse.updateDeltas();

    hierarchyModule->updateWorldMatrices(ecs);
    const std::vector<glm::mat4>& worldMatrices = hierarchyModule->getWorldMatrices();

    {
        jleGraphicsModule::UpdateContext graphicsUpdateContext{
            .in =    {   .screenX = windowX,
                            .screenY = windowY,
                            .worldMatrices = worldMatrices },
            .inOut = {  .ecs = ecs }
        };

        graphicsModule->update(graphicsUpdateContext);
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

    assert(hierarchyModule && graphicsModule);

    hierarchyModule->updateWorldMatrices(ecs);
    const std::vector<glm::mat4> &worldMatrices = hierarchyModule->getWorldMatrices();

    // Todo: read from window module here ?
    uint32_t windowX = 1024;
    uint32_t windowY = 768;

    {
        jleGraphicsModule::UpdateContext graphicsUpdateContext{
            .in = {.screenX = windowX,
                   .screenY = windowY,
                   .worldMatrices = worldMatrices},
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
    if (auto *windowModule = modules.getModule<jleWindowModule>()) {
        const int windowX = windowModule->window.getDimensions().framebufferWidth;
        const int windowY = windowModule->window.getDimensions().framebufferHeight;

        if (auto *graphicsModule = modules.getModule<jleGraphicsModule>()) {
            graphicsModule->render(windowX, windowY);
        }

        windowModule->window.updateWindow();
        return windowModule->window.windowShouldClose();
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