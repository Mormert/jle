// Copyright (c) 2022. Johan Lind

#include "ProfilerWindow.h"


jle::ProfilerWindow::ProfilerWindow(const std::string &window_name)
        : iEditorImGuiWindow{window_name} {
}

void jle::ProfilerWindow::Update(jleGameEngine &ge) {
    mProfilersWindow.Render();
}
