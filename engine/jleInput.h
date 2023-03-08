// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleKeyboardInput.h"
#include "jleMouseInput.h"

#include <memory>
#include <utility>

class jleInput {
public:
    ~jleInput() = default;

    jleInput(std::shared_ptr<jleKeyboardInput> ki,
             std::shared_ptr<jleMouseInput> mi)
        : keyboard{std::move(std::move(ki))}, mouse{std::move(std::move(mi))} {}

    std::shared_ptr<jleKeyboardInput> keyboard;
    std::shared_ptr<jleMouseInput> mouse;

    // Set to false if the input system should stop polling
    void inputEnabled(bool isEnabled) {
        _isInputEnabled = isEnabled;

        keyboard->isEnabled(isEnabled);
        mouse->isEnabled(isEnabled);
    }

    bool _isInputEnabled = true;
};
