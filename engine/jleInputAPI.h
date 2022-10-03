// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleKeyboardInput.h"
#include "jleMouseInput.h"

#include <memory>
#include <utility>

class jleInputAPI {
public:
    ~jleInputAPI() = default;

    jleInputAPI(std::shared_ptr<jleKeyboardInput> ki,
                std::shared_ptr<jleMouseInput> mi)
        : keyboard{std::move(std::move(ki))}, mouse{std::move(std::move(mi))} {}

    std::shared_ptr<jleKeyboardInput> keyboard;
    std::shared_ptr<jleMouseInput> mouse;

    // Set to false if the input system should stop polling
    void SetInputEnabled(bool isEnabled) {
        _isInputEnabled = isEnabled;

        keyboard->SetIsEnabled(isEnabled);
        mouse->SetIsEnabled(isEnabled);
    }

    bool _isInputEnabled = true;
};
