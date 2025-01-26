/*********************************************************************************************
 *               Copyright (c) 2023-2024 Johan Lind. All rights reserved.                    *
 *********************************************************************************************/

#include "GameTemplate.h"

#include "game/jleDefaultGameFunctions.h"
#include <jleGameEngine.h>
#include <modules/windowing/jleWindowModule.h>
#include <runtime/jleKickStarter.h>

int
main(int argc, char *argv[])
{
    jleCommandArguments commandArguments{argc, argv};

    jleGameEngine::EngineConstructConfig config{
        .windowCreator = std::make_unique<jleWindowModule>,
        .gameConfig = {
            .modulesCreator = GameTemplateFunctions::createModules,
            .ecsCreator = std::make_unique<jlECS::Debug::ECS_Debug>,
            .modulesInitialize = GameTemplateFunctions::modulesInitialize,
            .modulesUpdate = GameTemplateFunctions::modulesUpdate,
            .populateSerializationInterfaces = GameTemplateFunctions::populateSerializeableInterfaces,
            .modulesPreRender = jleDefaultGameFunctions::defaultPreRender,
            .modulesRender = jleDefaultGameFunctions::defaultRender,
            .modulesPostRender = jleDefaultGameFunctions::defaultPostRender
        }
    };

    auto engine = jleGameEngine{config};
    engine.run();

    return 0;
}
