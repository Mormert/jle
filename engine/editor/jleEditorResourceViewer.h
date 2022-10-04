// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleEditorImGuiWindowInterface.h"

class jleEditorResourceViewer : public iEditorImGuiWindow {
public:
    explicit jleEditorResourceViewer(const std::string &window_name);

    void update(jleGameEngine &ge) override;
};
