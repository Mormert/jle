// Copyright (c) 2023. Johan Lind

#pragma once

#include "game/jleGame.h"

// An empty game template with default modules and functions.

namespace GameTemplateFunctions
{
    std::unique_ptr<jleGameModules> createModules(bool gameRunning);
    void modulesInitialize(jleGameModules& modules, jlECS::ECS& ecs, jleSerializationContext& serializationContext);
    void modulesUpdate(jleGameModules& modules, jleEngineUpdateContext& ctx, jlECS::ECS& ecs);
    void modulesUpdateRenderablesOnly(jleGameModules& modules, jleEngineUpdateContext& ctx, jlECS::ECS& ecs);
    void populateSerializeableInterfaces(jleGameModules &modules, std::vector<jleSerializableInterface *> &interfaces);
}
