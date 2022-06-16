// Copyright (c) 2022. Johan Lind

#pragma once

#include "iEditorImGuiWindow.h"

#include "jleGameEngine.h"

namespace jle {
    class GameEditorWindow : public iEditorImGuiWindow {
    public:
        explicit GameEditorWindow(const std::string &window_name);

        void Update(jleGameEngine &ge) override;

        static inline GameEditorWindow* gGameEditorWindow;

        int GetWindowWidth() const;
        int GetWindowHeight() const;
        int GetWindowPosX() const;
        int GetWindowPosY() const;

    private:
        float mLastGameWindowWidth = 0.f, mLastGameWindowHeight = 0.f;
        int mWindowPositionX, mWindowPositionY;
        bool mWasFocused = false;
    };
}