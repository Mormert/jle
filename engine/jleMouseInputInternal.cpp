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
    return windowInternal->GetCursor().first - mScreenBeginX;
#else
    return windowInternal->GetCursor().first;
#endif
}

int jleMouseInputInternal::GetMouseY() {
#ifdef BUILD_EDITOR
    return windowInternal->GetCursor().second - mScreenBeginY;
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
    mScreenBeginX = x;
    mScreenBeginY = y;
}

void jleMouseInputInternal::SetScreenSize(int width, int height) {
    mScreenWidth = width;
    mScreenHeight = height;
}

void jleMouseInputInternal::SetPixelatedScreenSize(int width, int height) {
    mPixelatedScreenWidth = width;
    mPixelatedScreenHeight = height;
}

int jleMouseInputInternal::GetPixelatedMouseX() {
    const float ratio = float(mPixelatedScreenWidth) / float(mScreenWidth);
    return int(ratio * float(GetMouseX()));
}

int jleMouseInputInternal::GetPixelatedMouseY() {
    const float ratio = float(mPixelatedScreenHeight) / float(mScreenHeight);
    return int(ratio * float(GetMouseY()));
}

bool jleMouseInputInternal::GetMouseClick(int button) {
    if (input_enabled) {
        return windowInternal->GetMouseClick(button);
    }
    return false;
}
