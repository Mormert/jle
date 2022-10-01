// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleMouseInputInterface.h"
#include "jleWindowInternalAPIInterface.h"
#include "jleWindowLinkableInterface.h"

class jleMouseInputInternal : public jleMouseInputInterface,
                              public jleWindowLinkableInterface {
public:
    explicit jleMouseInputInternal(
        std::shared_ptr<jleWindowInternalAPIInterface> windowInternal);

    void LinkWindow(
        std::shared_ptr<jleWindowInternalAPIInterface> windowInternal) override;

    int GetMouseX() override;

    int GetMouseY() override;

    float GetMouseXDelta() override;

    float GetMouseYDelta() override;

    float GetScrollX() override;

    float GetScrollY() override;

    int GetPixelatedMouseX() override;

    int GetPixelatedMouseY() override;

    bool GetMouseClick(int button) override;

    void SetScreenBeginCoords(int x, int y);

    void SetScreenSize(int width, int height);

    void SetPixelatedScreenSize(int width, int height);

private:
    std::shared_ptr<jleWindowInternalAPIInterface> windowInternal;

    int _screenBeginX{0}, _screenBeginY{0};
    int _screenWidth, _screenHeight;
    int _pixelatedScreenWidth, _pixelatedScreenHeight;
};
