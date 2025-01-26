// Copyright (c) 2023. Johan Lind

#pragma once

#include <memory>

class jleEditorUpdateContext;
struct jleGameModules;

namespace GameTemplateFunctionsEditor
{
    std::unique_ptr<jleGameModules> createModules_Editor(bool gameRunning);
    void updateEditorGameModules(jleEditorUpdateContext &ctx);
}