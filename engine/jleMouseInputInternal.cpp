// Copyright (c) 2022. Johan Lind

#include "jleMouseInputInternal.h"

#include <GLFW/glfw3.h>
#include <utility>

jleMouseInputInternal::jleMouseInputInternal(
    std::shared_ptr<jleWindowInternalAPIInterface> windowInternal) {
    this->windowInternal = std::move(windowInternal);
}

void jleMouseInputInternal::LinkWindow(
    std::shared_ptr<jleWindowInternalAPIInterface> windowInternal) {
    this->windowInternal = windowInternal;
}

int jleMouseInputInternal::GetMouseX() {
#ifdef BUILD_EDITOR
    return windowInternal->GetCursor().first - _screenBeginX;
#else
    return windowInternal->GetCursor().first;
#endif
}

int jleMouseInputInternal::GetMouseY() {
#ifdef BUILD_EDITOR
    return windowInternal->GetCursor().second - _screenBeginY;
#else
    return windowInternal->GetCursor().second;
#endif
}

float jleMouseInputInternal::GetMouseXDelta() { return 0; }

float jleMouseInputInternal::GetMouseYDelta() { return 0; }

float jleMouseInputInternal::GetScrollX() {
    return windowInternal->GetScrollX();
}

float jleMouseInputInternal::GetScrollY() {
    return windowInternal->GetScrollY();
}

void jleMouseInputInternal::SetScreenBeginCoords(int x, int y) {
    _screenBeginX = x;
    _screenBeginY = y;
}

void jleMouseInputInternal::SetScreenSize(int width, int height) {
    _screenWidth = width;
    _screenHeight = height;
}

void jleMouseInputInternal::SetPixelatedScreenSize(int width, int height) {
    _pixelatedScreenWidth = width;
    _pixelatedScreenHeight = height;
}

int jleMouseInputInternal::GetPixelatedMouseX() {
    const float ratio = float(_pixelatedScreenWidth) / float(_screenWidth);
    return int(ratio * float(GetMouseX()));
}

int jleMouseInputInternal::GetPixelatedMouseY() {
    const float ratio = float(_pixelatedScreenHeight) / float(_screenHeight);
    return int(ratio * float(GetMouseY()));
}

bool jleMouseInputInternal::GetMouseClick(int button) {
    if (input_enabled) {
        return windowInternal->GetMouseClick(button);
    }
    return false;
}
