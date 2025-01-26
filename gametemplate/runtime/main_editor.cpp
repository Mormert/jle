/*********************************************************************************************
 *               Copyright (c) 2023-2024 Johan Lind. All rights reserved.                    *
 *********************************************************************************************/

#include "GameTemplate.h"
#include "GameTemplateEditor.h"
#include "game/jleDefaultGameFunctions.h"

#include <core/jleMalloc.h>
#include "core/jleCommandArguments.h"
#include <editor/jleEditor.h>
#include <editor/jleEditorWindow.h>


int
main(int argc, char *argv[])
{
    jleCommandArguments commandArguments{argc, argv};

    jleMalloc::InstallMemTrackingHooks();

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
            .populateSerializationInterfaces = GameTemplateFunctions::populateSerializeableInterfaces,
            .modulesPreRender = jleDefaultGameFunctions::defaultPreRender,
            .modulesRender = jleDefaultGameFunctions::defaultRender,
            .modulesPostRender = jleDefaultGameFunctions::defaultPostRender
        }
    };

    auto editor = jleEditor(editorConstructConfig, engineConstructConfig);
    editor.run();

    return 0;
}