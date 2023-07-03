// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR

#include "editor/jleEditorImGuiWindowInterface.h"

#include "jleGameEngine.h"

class jleGameEditorWindow : public iEditorImGuiWindow
{
public:
    explicit jleGameEditorWindow(const std::string &window_name);

    void update(jleGameEngine &ge) override;

    static inline jleGameEditorWindow *gGameEditorWindow;

    int width() const;

    int height() const;

    int x() const;

    int y() const;

private:
    float _lastGameWindowWidth = 0.f, _lastGameWindowHeight = 0.f;
    int _windowPositionX, _windowPositionY;
    bool _wasFocused = false;
};

#endif // BUILD_EDITOR
