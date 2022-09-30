// Copyright (c) 2022. Johan Lind

#pragma once

#include "editor/jleEditorImGuiWindowInterface.h"
#include "editor/jleEditorJsonToImgui.h"

#include <memory>

struct jleEditorSettings;

class jleEngineSettingsWindow : public iEditorImGuiWindow {
public:
    jleEngineSettingsWindow(const std::string& window_name,
                            const std::shared_ptr<jleGameSettings>& gs,
                            const std::shared_ptr<jleEditorSettings>& es);

    virtual void Update(jleGameEngine& ge) override;

private:
    std::shared_ptr<jleGameSettings> gameSettings;
    std::shared_ptr<jleEditorSettings> editorSettings;
    jleEditorJsonToImgui mJsonToImguiGS;
    jleEditorJsonToImgui mJsonToImguiES;
};
