// Copyright (c) 2022. Johan Lind

#include "jleMouseInput.h"

#include "jleWindow.h"
#include <GLFW/glfw3.h>
#include <utility>

jleMouseInput::jleMouseInput(
    std::shared_ptr<jleWindow> windowInternal) {
    this->windowInternal = std::move(windowInternal);
}

void jleMouseInput::linkWindow(
    std::shared_ptr<jleWindow> windowInternal) {
    this->windowInternal = windowInternal;
}

int jleMouseInput::mouseX() {
#ifdef BUILD_EDITOR
    return windowInternal->cursor().first - _screenBeginX;
#else
    return windowInternal->cursor().first;
#endif
}

int jleMouseInput::mouseY() {
#ifdef BUILD_EDITOR
    return windowInternal->cursor().second - _screenBeginY;
#else
    return windowInternal->cursor().second;
#endif
}

float jleMouseInput::mouseXDelta() { return 0; }

float jleMouseInput::mouseYDelta() { return 0; }

float jleMouseInput::GetScrollX() {
    return windowInternal->GetScrollX();
}

float jleMouseInput::GetScrollY() {
    return windowInternal->GetScrollY();
}

void jleMouseInput::screenBeginCoords(int x, int y) {
    _screenBeginX = x;
    _screenBeginY = y;
}

void jleMouseInput::screenSize(int width, int height) {
    _screenWidth = width;
    _screenHeight = height;
}

void jleMouseInput::pixelatedScreenSize(int width, int height) {
    _pixelatedScreenWidth = width;
    _pixelatedScreenHeight = height;
}

void jleMouseInput::isEnabled(bool value) { _isEnabled = value; }

int jleMouseInput::pixelatedMouseX() {
    const float ratio = float(_pixelatedScreenWidth) / float(_screenWidth);
    return int(ratio * float(mouseX()));
}

int jleMouseInput::pixelatedMouseY() {
    const float ratio = float(_pixelatedScreenHeight) / float(_screenHeight);
    return int(ratio * float(mouseY()));
}

bool jleMouseInput::mouseClick(int button) {
    if (_isEnabled) {
        return windowInternal->mouseClick(button);
    }
    return false;
}
