// Copyright (c) 2022. Johan Lind

#pragma once

#include "editor/jleEditorImGuiWindowInterface.h"

#include "jleGameEngine.h"

class jleGameEditorWindow : public iEditorImGuiWindow {
public:
    explicit jleGameEditorWindow(const std::string& window_name);

    void Update(jleGameEngine& ge) override;

    static inline jleGameEditorWindow *gGameEditorWindow;

    int GetWindowWidth() const;

    int GetWindowHeight() const;

    int GetWindowPosX() const;

    int GetWindowPosY() const;

private:
    float mLastGameWindowWidth = 0.f, mLastGameWindowHeight = 0.f;
    int mWindowPositionX, mWindowPositionY;
    bool mWasFocused = false;
};
