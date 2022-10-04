// Copyright (c) 2022. Johan Lind

#pragma once

#include <memory>

class jleWindow;

class jleMouseInput {
public:
    explicit jleMouseInput(std::shared_ptr<jleWindow> windowInternal);

    void linkWindow(std::shared_ptr<jleWindow> windowInternal);

    int mouseX();

    int mouseY();

    float mouseXDelta();

    float mouseYDelta();

    float GetScrollX();

    float GetScrollY();

    int pixelatedMouseX();

    int pixelatedMouseY();

    bool mouseClick(int button);

    void screenBeginCoords(int x, int y);

    void screenSize(int width, int height);

    void pixelatedScreenSize(int width, int height);

    void isEnabled(bool value);

private:
    std::shared_ptr<jleWindow> windowInternal;

    int _screenBeginX{0}, _screenBeginY{0};
    int _screenWidth, _screenHeight;
    int _pixelatedScreenWidth, _pixelatedScreenHeight;
    bool _isEnabled = true;
};
