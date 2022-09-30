// Copyright (c) 2022. Johan Lind

#pragma once

#include <string>

#include "jleGameEngine.h"

#include <memory>
#include <utility>
#include <vector>

class iEditorImGuiWindow {
public:
    explicit iEditorImGuiWindow(std::string window_name)
        : window_name{std::move(window_name)} {}

    virtual ~iEditorImGuiWindow() = default;
    ;

    virtual void Update(jleGameEngine& ge) = 0;

    virtual void OpenWindow() { isOpened = true; }

    virtual void CloseWindow() { isOpened = false; }

    bool GetOpened() const { return isOpened; }

    const std::string& GetWindowName() { return window_name; }

protected:
    std::string window_name;
    bool isOpened = true;
};