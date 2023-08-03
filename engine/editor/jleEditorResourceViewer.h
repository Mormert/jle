// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR

#include "jleEditorImGuiWindowInterface.h"

class jleEditorResourceViewer : public jleEditorWindowInterface
{
public:
    explicit jleEditorResourceViewer(const std::string &window_name);

    void update(jleGameEngine &ge) override;
};

#endif // BUILD_EDITOR
