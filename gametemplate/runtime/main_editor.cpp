/*********************************************************************************************
 *               Copyright (c) 2023-2024 Johan Lind. All rights reserved.                    *
 *********************************************************************************************/

#include "GameTemplate.h"
#include "GameTemplateEditor.h"

#include <editor/jleEditor.h>
#include <editor/jleEditorWindow.h>
#include <core/jleMalloc.h>

#include <runtime/jleKickStarter.h>

int
main(int argc, char *argv[])
{
    jleMalloc::InstallMemTrackingHooks();

    auto kickstarter = jleKickStarter{};

    jleEditor::EditorConstructConfig editorConstructConfig{
        .modulesUpdateRenderablesOnly = GameTemplateFunctions::modulesUpdateRenderablesOnly,
        .updateEditorGameModules = GameTemplateFunctionsEditor::updateEditorGameModules
    };

    jleGameEngine::EngineConstructConfig engineConstructConfig = {
        .windowCreator = std::make_unique<jleEditorWindow>,
        .gameConfig = {
            .modulesCreator = GameTemplateFunctionsEditor::createModules_Editor,
            .ecsCreator = std::make_unique<jlECS::Debug::ECS_Debug>,
            .modulesInitialize = GameTemplateFunctions::modulesInitialize,
            .modulesUpdate = GameTemplateFunctions::modulesUpdate,
            .populateSerializationInterfaces = GameTemplateFunctions::populateSerializeableInterfaces
        }
    };

    auto editor = std::make_unique<jleEditor>(editorConstructConfig, engineConstructConfig);
    kickstarter.kickStart(std::move(editor), argc, argv);
    return 0;
}