// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleKeyboardInputInternal.h"
#include "jleMouseInputInternal.h"

#include <memory>
#include <utility>

class jleInputAPI {
public:
    ~jleInputAPI() = default;

    jleInputAPI(std::shared_ptr<jleKeyboardInputInternal> ki,
                std::shared_ptr<jleMouseInputInternal> mi)
        : keyboard{std::move(std::move(ki))}, mouse{std::move(std::move(mi))} {}

    std::shared_ptr<jleKeyboardInputInternal> keyboard;
    std::shared_ptr<jleMouseInputInternal> mouse;

    // Set to false if the input system should stop polling
    void SetInputEnabled(bool isEnabled) {
        _isInputEnabled = isEnabled;

        keyboard->SetIsEnabled(isEnabled);
        mouse->SetIsEnabled(isEnabled);
    }

    bool _isInputEnabled = true;
};
