// Copyright (c) 2023. Johan Lind

#include "jleKeyboardInput.h"
#include "jleWindow.h"

#include <utility>

jleKeyboardInput::jleKeyboardInput(std::shared_ptr<jleWindow> window) {
    this->windowInternal = std::move(window);
}

void jleKeyboardInput::linkWindow(std::shared_ptr<jleWindow> window) {
    this->windowInternal = std::move(window);
}

bool jleKeyboardInput::keyPressed(int key) {
    if (_isEnabled) {
        return windowInternal->keyPressed(key);
    }
    return false;
}

bool jleKeyboardInput::keyReleased(int key) {
    if (_isEnabled) {
        return windowInternal->keyReleased(key);
    }
    return false;
}

bool jleKeyboardInput::keyDown(int key, bool ignoreInputEnabled) {
    if (ignoreInputEnabled) {
        return windowInternal->key(key);
    }
    if (_isEnabled) {
        return windowInternal->key(key);
    }
    return false;
}

void jleKeyboardInput::isEnabled(bool value) { _isEnabled = value; }
