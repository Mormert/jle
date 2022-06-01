// Copyright (c) 2022. Johan Lind

#pragma once

#include "iEditorImGuiWindow.h"
#include "EditorJsonToImgui.h"

namespace jle {
    class EditorSceneObjectsWindow : public iEditorImGuiWindow {
    public:
        explicit EditorSceneObjectsWindow(const std::string &window_name);

        void Update(jleGameEngine &ge) override;

    private:
        EditorJsonToImgui mJsonToImgui;
    };
}