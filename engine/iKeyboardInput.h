// Copyright (c) 2022. Johan Lind

#pragma once

#include "iInput.h"

namespace jle {
    class iKeyboardInput : public iInput {
    public:
        ~iKeyboardInput() override = default;

        // Returns true if a specified key is pressed
        virtual bool GetKeyPressed(char key) = 0;

        // Returns true if a specified key is released
        virtual bool GetKeyReleased(char key) = 0;

        // Returns true if a speicifed key is down
        virtual bool GetKeyDown(char key) = 0;
    };
}