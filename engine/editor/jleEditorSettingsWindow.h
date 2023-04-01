// Copyright (c) 2023. Johan Lind

#pragma once

#include "editor/jleEditorImGuiWindowInterface.h"
#include "editor/jleEditorJsonToImgui.h"

#include <memory>

class jleEditorSettingsWindow : public iEditorImGuiWindow {
public:
    jleEditorSettingsWindow(const std::string &window_name);

    virtual void update(jleGameEngine &ge) override;
};
