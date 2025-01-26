/*********************************************************************************************
 *               Copyright (c) 2023-2024 Johan Lind. All rights reserved.                    *
 *********************************************************************************************/

#include <game/loader/editor/jleGameModulesLoaderEditor.h>
#include <editor/jleEditor.h>
#include <editor/jleEditorWindow.h>
#include <runtime/jleKickStarter.h>

int
main(int argc, char *argv[])
{
    auto kickstarter = jleKickStarter{};

    jleGameEngine::EngineConstructConfig config{
        .window = std::make_unique<jleEditorWindow>(),
        .gameConfig = {
            .gameCreator = std::make_unique<jleGame>,
            .modulesCreator = jleModuleLoading::createDefaultModules_Editor,
            .ecsCreator = std::make_unique<jlECS::Debug::ECS_Debug> }
    };

    auto editor = std::make_unique<jleEditor>(config);
    kickstarter.kickStart(std::move(editor), argc, argv);
    return 0;
}