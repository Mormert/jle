// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleWindowInternalAPIInterface.h"
#include "jleWindowLinkableInterface.h"

class jleMouseInputInternal : public jleWindowLinkableInterface {
public:
    explicit jleMouseInputInternal(
        std::shared_ptr<jleWindowInternalAPIInterface> windowInternal);

    void LinkWindow(
        std::shared_ptr<jleWindowInternalAPIInterface> windowInternal) override;

    int GetMouseX();

    int GetMouseY();

    float GetMouseXDelta();

    float GetMouseYDelta();

    float GetScrollX();

    float GetScrollY();

    int GetPixelatedMouseX();

    int GetPixelatedMouseY();

    bool GetMouseClick(int button);

    void SetScreenBeginCoords(int x, int y);

    void SetScreenSize(int width, int height);

    void SetPixelatedScreenSize(int width, int height);

    void SetIsEnabled(bool value);

private:
    std::shared_ptr<jleWindowInternalAPIInterface> windowInternal;

    int _screenBeginX{0}, _screenBeginY{0};
    int _screenWidth, _screenHeight;
    int _pixelatedScreenWidth, _pixelatedScreenHeight;
    bool _isEnabled = true;
};
