// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleEditorImGuiWindowInterface.h"
#include "jleProfiler.h"

class jleEditorProfilerWindow : public iEditorImGuiWindow {
public:
    explicit jleEditorProfilerWindow(const std::string& window_name);

    void Update(jleGameEngine& ge) override;

private:
    void DrawProfilerRecursive(uint32_t index);
};
