// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR

#include "editor/jleEditorImGuiWindowInterface.h"
#include "editor/jleEditorJsonToImgui.h"

#include <memory>

class jleEditorSettingsWindow : public iEditorImGuiWindow {
public:
    jleEditorSettingsWindow(const std::string &window_name);

    virtual void update(jleGameEngine &ge) override;
};

#endif // BUILD_EDITOR
