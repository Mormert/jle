// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleInputInterface.h"

#include "jleKeyboardInputInterface.h"
#include "jleMouseInputInterface.h"

#include <memory>
#include <utility>

class jleInputAPI : public jleInputInterface {
public:
    ~jleInputAPI() override = default;

    jleInputAPI(std::shared_ptr<jleKeyboardInputInterface> ki,
             std::shared_ptr<jleMouseInputInterface> mi)
        : keyboard{std::move(std::move(ki))}, mouse{std::move(std::move(mi))} {}

    std::shared_ptr<jleKeyboardInputInterface> keyboard;
    std::shared_ptr<jleMouseInputInterface> mouse;

    // Set to false if the input system should stop polling
    void SetInputEnabled(bool enabled) override {
        jleInputInterface::SetInputEnabled(enabled);

        keyboard->SetInputEnabled(enabled);
        mouse->SetInputEnabled(enabled);
    }
};
