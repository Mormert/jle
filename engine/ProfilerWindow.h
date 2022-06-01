// Copyright (c) 2022. Johan Lind

#pragma once

#include "iEditorImGuiWindow.h"

#include "jleGameEngine.h"
#include "ImGuiProfilerRenderer.h"

namespace jle {
    class ProfilerWindow : public iEditorImGuiWindow {
    public:
        ProfilerWindow(const std::string &window_name);

        virtual void Update(jleGameEngine &ge) override;

        ProfilersWindow mProfilersWindow;
    };

}