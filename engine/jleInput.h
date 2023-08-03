// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleKeyboardInput.h"
#include "jleMouseInput.h"
#include <memory>

class jleWindow;

class jleInput
{
public:
    ~jleInput() = default;

    explicit jleInput(const std::shared_ptr<jleWindow> &window);

    const std::unique_ptr<jleKeyboardInput> keyboard;
    const std::unique_ptr<jleMouseInput> mouse;

    void setInputEnabled(bool isEnabled);

    bool getInputEnabled() const;

private:
    bool _isInputEnabled = true;
};
