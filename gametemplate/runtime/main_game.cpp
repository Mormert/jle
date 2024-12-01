/*********************************************************************************************
 *               Copyright (c) 2023-2024 Johan Lind. All rights reserved.                    *
 *********************************************************************************************/

#include <jleGameEngine.h>
#include <runtime/jleKickStarter.h>

#include "GameTemplate.h"

int
main(int argc, char *argv[])
{
    auto kickstarter = jleKickStarter{};
    kickstarter.kickStart<jleGame>(std::make_unique<jleGameEngine>(), argc, argv);
    return 0;
}