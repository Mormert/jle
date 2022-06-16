// Copyright (c) 2022. Johan Lind

#include "MouseInputInternal.h"

#include <utility>

namespace jle {
    MouseInputInternal::MouseInputInternal(std::shared_ptr<iWindowInternalAPI> windowInternal) {
        this->windowInternal = std::move(windowInternal);
    }

    void MouseInputInternal::LinkWindow(std::shared_ptr<iWindowInternalAPI> windowInternal) {
        this->windowInternal = windowInternal;
    }

    int MouseInputInternal::GetMouseX() {
        if (input_enabled) {
#ifdef BUILD_EDITOR
            return windowInternal->GetCursor().first - mScreenBeginX;
#else
            return windowInternal->GetCursor().first;
#endif
        }
        return 0;
    }

    int MouseInputInternal::GetMouseY() {
        if (input_enabled) {
#ifdef BUILD_EDITOR
            return windowInternal->GetCursor().second - mScreenBeginY;
#else
            return windowInternal->GetCursor().second;
#endif
        }
        return 0;
    }

    float MouseInputInternal::GetMouseXDelta() {
        return 0;
    }

    float MouseInputInternal::GetMouseYDelta() {
        return 0;
    }

    float MouseInputInternal::GetScrollX() {
        return windowInternal->GetScrollX();
    }

    float MouseInputInternal::GetScrollY() {
        return windowInternal->GetScrollY();
    }

    void MouseInputInternal::SetScreenBeginCoords(int x, int y) {
        mScreenBeginX = x;
        mScreenBeginY = y;
    }

    void MouseInputInternal::SetScreenSize(int width, int height) {
        mScreenWidth = width;
        mScreenHeight = height;
    }

    void MouseInputInternal::SetPixelatedScreenSize(int width, int height) {
        mPixelatedScreenWidth = width;
        mPixelatedScreenHeight = height;
    }

    int MouseInputInternal::GetPixelatedMouseX() {
        const float ratio = float(mPixelatedScreenWidth) / float(mScreenWidth);
        return int(ratio * float(GetMouseX()));
    }

    int MouseInputInternal::GetPixelatedMouseY() {
        const float ratio = float(mPixelatedScreenHeight) / float(mScreenHeight);
        return int(ratio * float(GetMouseY()));
    }
}