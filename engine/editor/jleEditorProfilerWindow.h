// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleEditorImGuiWindowInterface.h"
#include "jleProfiler.h"

class jleEditorProfilerWindow : public iEditorImGuiWindow {
public:
    explicit jleEditorProfilerWindow(const std::string &window_name);

    void update(jleGameEngine &ge) override;

private:
    void drawProfilerRecursive(uint32_t index);
};
