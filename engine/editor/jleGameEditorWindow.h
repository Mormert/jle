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

    int windowPosX() const;

    int windowPosY() const;

private:
    float _lastGameWindowWidth = 0.f, _lastGameWindowHeight = 0.f;
    int _windowPositionX, _windowPositionY;
    bool _wasFocused = false;
};
