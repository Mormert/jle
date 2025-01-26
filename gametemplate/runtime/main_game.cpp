/*********************************************************************************************
 *               Copyright (c) 2023-2024 Johan Lind. All rights reserved.                    *
 *********************************************************************************************/

#include "GameTemplate.h"

#include <jleGameEngine.h>
#include <modules/windowing/jleWindow.h>
#include <runtime/jleKickStarter.h>

int
main(int argc, char *argv[])
{
    auto kickstarter = jleKickStarter{};

    jleGameEngine::EngineConstructConfig config{
        .windowCreator = std::make_unique<jleWindow>,
        .gameConfig = {
            .modulesCreator = GameTemplateFunctions::createModules,
            .ecsCreator = std::make_unique<jlECS::Debug::ECS_Debug>,
            .modulesInitialize = GameTemplateFunctions::modulesInitialize,
            .modulesUpdate = GameTemplateFunctions::modulesUpdate,
            .populateSerializationInterfaces = GameTemplateFunctions::populateSerializeableInterfaces
        }
    };

    auto engine = std::make_unique<jleGameEngine>(config);
    kickstarter.kickStart(std::move(engine), argc, argv);
    return 0;
}
