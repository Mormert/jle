// Copyright (c) 2022. Johan Lind

#include "KeyboardInputInternal.h"
#include <GLFW/glfw3.h>

namespace jle {
    KeyboardInputInternal::KeyboardInputInternal(std::shared_ptr<iWindowInternalAPI> windowInternal) {
        this->windowInternal = windowInternal;
    }

    void KeyboardInputInternal::LinkWindow(std::shared_ptr<iWindowInternalAPI> windowInternal) {
        this->windowInternal = windowInternal;
    }


    bool KeyboardInputInternal::GetKeyPressed(char key) {
        if (input_enabled) {
            return windowInternal->GetKeyPressed(key);
        }
        return false;
    }

    bool KeyboardInputInternal::GetKeyReleased(char key) {
        if (input_enabled) {
            return windowInternal->GetKeyReleased(key);
        }
        return false;
    }

    bool KeyboardInputInternal::GetKeyDown(char key) {
        if (input_enabled) {
            return windowInternal->GetKey(key);
        }
        return false;
    }
}