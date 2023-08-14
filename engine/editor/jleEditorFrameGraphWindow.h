// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR

#include "jleEditorImGuiWindowInterface.h"

class jleEditorFrameGraphWindow : public jleEditorWindowInterface
{
public:
    explicit jleEditorFrameGraphWindow(const std::string &window_name);

    void update(jleGameEngine &ge) override;

private:
};

#endif // BUILD_EDITOR