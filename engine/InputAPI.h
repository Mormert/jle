// Copyright (c) 2022. Johan Lind

#pragma once

#include "iInput.h"

#include "iKeyboardInput.h"
#include "iMouseInput.h"

#include <memory>
#include <utility>
#include <utility>

namespace jle {
    class InputAPI : public iInput {
    public:
        ~InputAPI() override = default;

        InputAPI(std::shared_ptr<iKeyboardInput> ki, std::shared_ptr<iMouseInput> mi) : keyboard{
                std::move(std::move(ki))}, mouse{std::move(std::move(mi))} {}

        std::shared_ptr<iKeyboardInput> keyboard;
        std::shared_ptr<iMouseInput> mouse;

        // Set to false if the input system should stop polling
        void SetInputEnabled(bool enabled) override {
            iInput::SetInputEnabled(enabled);

            keyboard->SetInputEnabled(enabled);
            mouse->SetInputEnabled(enabled);
        }

        /*// Gets the X mouse position
        virtual int GetMouseX() noexcept;
        // Gets the X mouse position
        virtual int GetMouseY() noexcept;

        // Gets the X position in the world where the mouse hovers over
        virtual int GetMouseWorldX() noexcept;
        // Gets the Y position in the world where the mouse hovers over
        virtual int GetMouseWorldY() noexcept;*/


    };
}