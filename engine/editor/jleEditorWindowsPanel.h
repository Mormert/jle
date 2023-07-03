// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR

#include "jleEditorGameControllerWidget.h"
#include "jleEditorImGuiWindowInterface.h"

class jleEditorWindowsPanel : public iEditorImGuiWindow {
public:
    explicit jleEditorWindowsPanel(const std::string &window_name);

    void update(jleGameEngine &ge) override;

    void addWindow(std::shared_ptr<iEditorImGuiWindow> window);

    inline void menuButtonsupdate(jleGameEngine &ge);

    inline void dockspaceupdate(jleGameEngine &ge);

private:
    std::vector<std::shared_ptr<iEditorImGuiWindow>> windows;

    jleEditorGameControllerWidget _gameController;
};

#endif // BUILD_EDITOR
