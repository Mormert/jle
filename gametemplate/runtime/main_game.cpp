/*********************************************************************************************
 *               Copyright (c) 2023-2024 Johan Lind. All rights reserved.                    *
 *********************************************************************************************/

#include <game/loader/jleGameModulesLoader.h>
#include <jleGameEngine.h>
#include <modules/windowing/jleWindow.h>
#include <runtime/jleKickStarter.h>

#include "GameTemplate.h"

int
main(int argc, char *argv[])
{
    auto kickstarter = jleKickStarter{};

    jleGameEngine::EngineConstructConfig config{
        .window = std::make_unique<jleWindow>(),
        .gameConfig = {
            .gameCreator = std::make_unique<jleGame>,
            .modulesCreator = jleModuleLoading::createDefaultModules,
            .ecsCreator = std::make_unique<jlECS::ECS> }
    };

    auto engine = std::make_unique<jleGameEngine>(config);
    kickstarter.kickStart(std::move(engine), argc, argv);
    return 0;
}
