// Copyright (c) 2022. Johan Lind

#include "jleKeyboardInput.h"
#include "jleWindow.h"
#include <GLFW/glfw3.h>

jleKeyboardInput::jleKeyboardInput(std::shared_ptr<jleWindow> windowInternal) {
    this->windowInternal = windowInternal;
}

void jleKeyboardInput::linkWindow(std::shared_ptr<jleWindow> windowInternal) {
    this->windowInternal = windowInternal;
}

bool jleKeyboardInput::keyPressed(char key) {
    if (_isEnabled) {
        return windowInternal->keyPressed(key);
    }
    return false;
}

bool jleKeyboardInput::keyReleased(char key) {
    if (_isEnabled) {
        return windowInternal->keyReleased(key);
    }
    return false;
}

bool jleKeyboardInput::keyDown(char key, bool ignoreInputEnabled) {
    if (ignoreInputEnabled) {
        return windowInternal->key(key);
    }
    if (_isEnabled) {
        return windowInternal->key(key);
    }
    return false;
}

void jleKeyboardInput::isEnabled(bool value) { _isEnabled = value; }
