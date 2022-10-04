// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleEditorGameControllerWidget.h"
#include "jleEditorImGuiWindowInterface.h"

class jleEditorWindowsPanel : public iEditorImGuiWindow {
public:
    explicit jleEditorWindowsPanel(const std::string& window_name);

    void Update(jleGameEngine& ge) override;

    void addWindow(std::shared_ptr<iEditorImGuiWindow> window);

    inline void menuButtonsUpdate(jleGameEngine& ge);

    inline void dockspaceUpdate(jleGameEngine& ge);

private:
    std::vector<std::shared_ptr<iEditorImGuiWindow>> windows;

    jleEditorGameControllerWidget _gameController;
};
