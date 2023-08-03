// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR

#include "jleEditorImGuiWindowInterface.h"
#include "jleProfiler.h"

class jleEditorProfilerWindow : public jleEditorWindowInterface
{
public:
    explicit jleEditorProfilerWindow(const std::string &window_name);

    void update(jleGameEngine &ge) override;

private:
    void drawProfilerRecursive(uint32_t index);
};

#endif // BUILD_EDITOR
