// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleEditorImGuiWindowInterface.h"
#include "jleTexture.h"

class jleEditorGameControllerWidget : public iEditorImGuiWindow {
public:
    explicit jleEditorGameControllerWidget(const std::string &window_name);

    void update(jleGameEngine &ge) override;
};
