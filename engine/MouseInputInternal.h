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

        int GetPixelatedMouseX() override;
        int GetPixelatedMouseY() override;

        void SetScreenBeginCoords(int x, int y);
        void SetScreenSize(int width, int height);
        void SetPixelatedScreenSize(int width, int height);

    private:
        std::shared_ptr<iWindowInternalAPI> windowInternal;

        int mScreenBeginX{0}, mScreenBeginY{0};
        int mScreenWidth, mScreenHeight;
        int mPixelatedScreenWidth, mPixelatedScreenHeight;
    };
}