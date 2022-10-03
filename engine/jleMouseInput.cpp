// Copyright (c) 2022. Johan Lind

#include "jleMouseInput.h"

#include "jleWindow.h"
#include <GLFW/glfw3.h>
#include <utility>

jleMouseInput::jleMouseInput(
    std::shared_ptr<jleWindow> windowInternal) {
    this->windowInternal = std::move(windowInternal);
}

void jleMouseInput::LinkWindow(
    std::shared_ptr<jleWindow> windowInternal) {
    this->windowInternal = windowInternal;
}

int jleMouseInput::GetMouseX() {
#ifdef BUILD_EDITOR
    return windowInternal->GetCursor().first - _screenBeginX;
#else
    return windowInternal->GetCursor().first;
#endif
}

int jleMouseInput::GetMouseY() {
#ifdef BUILD_EDITOR
    return windowInternal->GetCursor().second - _screenBeginY;
#else
    return windowInternal->GetCursor().second;
#endif
}

float jleMouseInput::GetMouseXDelta() { return 0; }

float jleMouseInput::GetMouseYDelta() { return 0; }

float jleMouseInput::GetScrollX() {
    return windowInternal->GetScrollX();
}

float jleMouseInput::GetScrollY() {
    return windowInternal->GetScrollY();
}

void jleMouseInput::SetScreenBeginCoords(int x, int y) {
    _screenBeginX = x;
    _screenBeginY = y;
}

void jleMouseInput::SetScreenSize(int width, int height) {
    _screenWidth = width;
    _screenHeight = height;
}

void jleMouseInput::SetPixelatedScreenSize(int width, int height) {
    _pixelatedScreenWidth = width;
    _pixelatedScreenHeight = height;
}

void jleMouseInput::SetIsEnabled(bool value) { _isEnabled = value; }

int jleMouseInput::GetPixelatedMouseX() {
    const float ratio = float(_pixelatedScreenWidth) / float(_screenWidth);
    return int(ratio * float(GetMouseX()));
}

int jleMouseInput::GetPixelatedMouseY() {
    const float ratio = float(_pixelatedScreenHeight) / float(_screenHeight);
    return int(ratio * float(GetMouseY()));
}

bool jleMouseInput::GetMouseClick(int button) {
    if (_isEnabled) {
        return windowInternal->GetMouseClick(button);
    }
    return false;
}
