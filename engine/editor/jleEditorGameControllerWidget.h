// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR

#include "jleEditorImGuiWindowInterface.h"
#include "jleTexture.h"

class jleEditorGameControllerWidget : public iEditorImGuiWindow {
public:
    explicit jleEditorGameControllerWidget(const std::string &window_name);

    void update(jleGameEngine &ge) override;
};

#endif // BUILD_EDITOR
