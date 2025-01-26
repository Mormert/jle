// Copyright (c) 2023. Johan Lind

#include "GameTemplateEditor.h"
#include "game/editor/jleDefaultGameEditorFunctions.h"
#include "modules/jleGameModules.h"

std::unique_ptr<jleGameModules>
GameTemplateFunctionsEditor::createModules_Editor(bool gameRunning)
{
    return jleDefaultGameEditorFunctions::createDefaultModules_Editor(gameRunning);
}

void
GameTemplateFunctionsEditor::updateEditorGameModules(jleEditorUpdateContext &ctx)
{
    jleDefaultGameEditorFunctions::updateEditorGameModules(ctx);
}