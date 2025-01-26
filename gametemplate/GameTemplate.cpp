// Copyright (c) 2023. Johan Lind

#include "GameTemplate.h"
#include "game/jleDefaultGameFunctions.h"

std::unique_ptr<jleGameModules>
GameTemplateFunctions::createModules(bool gameRunning)
{
    return jleDefaultGameFunctions::createDefaultModules(gameRunning);
}

void
GameTemplateFunctions::modulesInitialize(jleGameModules &modules, jlECS::ECS &ecs, jleSerializationContext &serializationContext)
{
    jleDefaultGameFunctions::defaultModulesInitialize(modules, ecs, serializationContext);
}

void
GameTemplateFunctions::modulesUpdate(jleGameModules &modules, jleEngineUpdateContext &ctx, jlECS::ECS &ecs)
{
    jleDefaultGameFunctions::defaultModulesUpdate(modules, ctx, ecs);
}

void
GameTemplateFunctions::modulesUpdateRenderablesOnly(jleGameModules &modules, jleEngineUpdateContext &ctx, jlECS::ECS &ecs)
{
    jleDefaultGameFunctions::defaultModulesUpdateRenderablesOnly(modules, ctx, ecs);
}

void
GameTemplateFunctions::populateSerializeableInterfaces(jleGameModules &modules, std::vector<jleSerializableInterface *> &interfaces)
{
    jleDefaultGameFunctions::defaultPopulateSerializeableInterfaces(modules, interfaces);
}