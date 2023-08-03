// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR

#include "editor/jleEditorImGuiWindowInterface.h"
#include <memory>

class jleEditorSettingsWindow : public jleEditorWindowInterface
{
public:
    explicit jleEditorSettingsWindow(const std::string &window_name);

    void update(jleGameEngine &ge) override;
};

#endif // BUILD_EDITOR
