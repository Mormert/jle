// Copyright (c) 2022. Johan Lind

#pragma once

#include "iMouseInput.h"
#include "iWindowLinkable.h"
#include "iWindowInternalAPI.h"

namespace jle {
    class MouseInputInternal : public iMouseInput, public iWindowLinkable {
    public:
        explicit MouseInputInternal(std::shared_ptr<iWindowInternalAPI> windowInternal);

        void LinkWindow(std::shared_ptr<iWindowInternalAPI> windowInternal) override;

        int GetMouseX() override;

        int GetMouseY() override;

        float GetMouseXDelta() override;

        float GetMouseYDelta() override;

        float GetScrollX() override;

        float GetScrollY() override;

    private:
        std::shared_ptr<iWindowInternalAPI> windowInternal;
    };
}