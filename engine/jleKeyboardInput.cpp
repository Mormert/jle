// Copyright (c) 2022. Johan Lind

#include "jleKeyboardInput.h"
#include "jleWindow_GLFW_OpenGL.h"
#include <GLFW/glfw3.h>

jleKeyboardInput::jleKeyboardInput(
    std::shared_ptr<jleWindow_GLFW_OpenGL> windowInternal) {
    this->windowInternal = windowInternal;
}

void jleKeyboardInput::LinkWindow(
    std::shared_ptr<jleWindow_GLFW_OpenGL> windowInternal) {
    this->windowInternal = windowInternal;
}

bool jleKeyboardInput::GetKeyPressed(char key) {
    if (_isEnabled) {
        return windowInternal->GetKeyPressed(key);
    }
    return false;
}

bool jleKeyboardInput::GetKeyReleased(char key) {
    if (_isEnabled) {
        return windowInternal->GetKeyReleased(key);
    }
    return false;
}

bool jleKeyboardInput::GetKeyDown(char key, bool ignoreInputEnabled) {
    if (ignoreInputEnabled) {
        return windowInternal->GetKey(key);
    }
    if (_isEnabled) {
        return windowInternal->GetKey(key);
    }
    return false;
}

void jleKeyboardInput::SetIsEnabled(bool value) { _isEnabled = value; }
