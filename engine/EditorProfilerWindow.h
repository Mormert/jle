// Copyright (c) 2022. Johan Lind


#ifndef EDITORPROFILERWINDOW_H
#define EDITORPROFILERWINDOW_H

#include "iEditorImGuiWindow.h"
#include "jleProfiler.h"

namespace jle {
    class EditorProfilerWindow : public iEditorImGuiWindow {
    public:
        explicit EditorProfilerWindow(const std::string &window_name);

        void Update(jleGameEngine &ge) override;

    private:
        void DrawProfilerRecursive(uint32_t index);
    };
}

#endif //EDITORPROFILERWINDOW_H
