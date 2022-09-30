// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleEditorGameControllerWidget.h"
#include "jleEditorImGuiWindowInterface.h"

class jleEditorWindowsPanel : public iEditorImGuiWindow {
public:
    explicit jleEditorWindowsPanel(const std::string& window_name);

    void Update(jleGameEngine& ge) override;

    void AddWindow(std::shared_ptr<iEditorImGuiWindow> window);

    inline void MenuButtonsUpdate(jleGameEngine& ge);

    inline void DockspaceUpdate(jleGameEngine& ge);

private:
    std::vector<std::shared_ptr<iEditorImGuiWindow>> windows;

    jleEditorGameControllerWidget mGameController;
};
