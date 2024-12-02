/*********************************************************************************************
 *               Copyright (c) 2023-2024 Johan Lind. All rights reserved.                    *
 *********************************************************************************************/

#include <jleGameEngine.h>
#include <modules/windowing/jleWindow.h>
#include <runtime/jleKickStarter.h>

#include "GameTemplate.h"

int
main(int argc, char *argv[])
{
    auto kickstarter = jleKickStarter{};
    auto window = std::make_unique<jleWindow>();
    kickstarter.kickStart<jleGame>(std::make_unique<jleGameEngine>(std::move(window)), argc, argv);
    return 0;
}