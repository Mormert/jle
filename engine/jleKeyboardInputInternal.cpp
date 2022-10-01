// Copyright (c) 2022. Johan Lind

#include "jleKeyboardInputInternal.h"
#include <GLFW/glfw3.h>

jleKeyboardInputInternal::jleKeyboardInputInternal(
    std::shared_ptr<jleWindowInternalAPIInterface> windowInternal) {
    this->windowInternal = windowInternal;
}

void jleKeyboardInputInternal::LinkWindow(
    std::shared_ptr<jleWindowInternalAPIInterface> windowInternal) {
    this->windowInternal = windowInternal;
}

bool jleKeyboardInputInternal::GetKeyPressed(char key) {
    if (_isEnabled) {
        return windowInternal->GetKeyPressed(key);
    }
    return false;
}

bool jleKeyboardInputInternal::GetKeyReleased(char key) {
    if (_isEnabled) {
        return windowInternal->GetKeyReleased(key);
    }
    return false;
}

bool jleKeyboardInputInternal::GetKeyDown(char key, bool ignoreInputEnabled) {
    if (ignoreInputEnabled) {
        return windowInternal->GetKey(key);
    }
    if (_isEnabled) {
        return windowInternal->GetKey(key);
    }
    return false;
}

void jleKeyboardInputInternal::SetIsEnabled(bool value) { _isEnabled = value; }
