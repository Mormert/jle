/*********************************************************************************************
 *               Copyright (c) 2023-2024 Johan Lind. All rights reserved.                    *
 *********************************************************************************************/

#include <editor/jleEditor.h>
#include <editor/jleEditorWindow.h>
#include <runtime/jleKickStarter.h>

#include "GameTemplate.h"

int
main(int argc, char *argv[])
{
    auto kickstarter = jleKickStarter{};
    auto window = std::make_unique<jleEditorWindow>();
    kickstarter.kickStart<jleGame>(std::make_unique<jleEditor>(std::move(window)), argc, argv);
    return 0;
}