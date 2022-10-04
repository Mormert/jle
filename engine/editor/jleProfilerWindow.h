// Copyright (c) 2022. Johan Lind

#pragma once

#include "editor/jleEditorImGuiWindowInterface.h"

#include "jleGameEngine.h"
#include "jleImGuiProfilerRenderer.h"

class jleProfilerWindow : public iEditorImGuiWindow {
public:
    jleProfilerWindow(const std::string& window_name);

    virtual void update(jleGameEngine& ge) override;

    profilersWindow _profilersWindow;
};
