// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR


#include "jleGameEngine.h"
#include "ImGui/imgui.h"

#include <string>
#include <memory>
#include <utility>
#include <vector>

class jleEditorWindowInterface
{
public:
    explicit jleEditorWindowInterface(std::string window_name)
        : window_name{std::move(window_name)} {}

    virtual ~jleEditorWindowInterface() = default;

    virtual void update(jleGameEngine &ge) = 0;

    virtual void openWindow() { isOpened = true; }

    virtual void closeWindow() { isOpened = false; }

    [[nodiscard]] bool opened() const { return isOpened; }

    const std::string &windowName() { return window_name; }

protected:
    std::string window_name;
    bool isOpened = true;
};

#endif // BUILD_EDITOR
