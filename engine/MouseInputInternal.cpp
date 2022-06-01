// Copyright (c) 2022. Johan Lind

#include "MouseInputInternal.h"

namespace jle {
    MouseInputInternal::MouseInputInternal(std::shared_ptr<iWindowInternalAPI> windowInternal) {
        this->windowInternal = windowInternal;
    }

    void MouseInputInternal::LinkWindow(std::shared_ptr<iWindowInternalAPI> windowInternal) {
        this->windowInternal = windowInternal;
    }

    int MouseInputInternal::GetMouseX() {
        if (input_enabled) {
            return windowInternal->GetCursor().first;
        }
        return 0;
    }

    int MouseInputInternal::GetMouseY() {
        if (input_enabled) {
            return windowInternal->GetCursor().second;
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
}