// Copyright (c) 2022. Johan Lind

#include "jleProfilerWindow.h"

jleProfilerWindow::jleProfilerWindow(const std::string& window_name)
    : iEditorImGuiWindow{window_name} {}

void jleProfilerWindow::update(jleGameEngine& ge) { _profilersWindow.render(); }
