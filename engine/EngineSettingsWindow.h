// Copyright (c) 2022. Johan Lind

#pragma once

#include "iEditorImGuiWindow.h"
#include "EditorJsonToImgui.h"

#include <memory>

namespace jle {

    struct jleEditorSettings;

    class EngineSettingsWindow : public iEditorImGuiWindow {
    public:
        EngineSettingsWindow(const std::string &window_name, const std::shared_ptr<jleGameSettings> &gs,
                             const std::shared_ptr<jleEditorSettings> &es);

        virtual void Update(jleGameEngine &ge) override;

    private:
        std::shared_ptr<jleGameSettings> gameSettings;
        std::shared_ptr<jleEditorSettings> editorSettings;
        EditorJsonToImgui mJsonToImguiGS;
        EditorJsonToImgui mJsonToImguiES;
    };

}