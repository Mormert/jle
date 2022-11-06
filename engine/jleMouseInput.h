// Copyright (c) 2022. Johan Lind

#pragma once

#include <memory>

class jleWindow;

class jleMouseInput
{
public:
    explicit jleMouseInput(std::shared_ptr<jleWindow> windowInternal);

    void linkWindow(std::shared_ptr<jleWindow> windowInternal);

    void updateDeltas();

    int mouseX();

    int mouseY();

    float xDelta();

    float yDelta();

    float scrollX();

    float scrollY();

    int pixelatedMouseX();

    int pixelatedMouseY();

    bool mouseClick(int button);

    void setScreenBeginCoords(int x, int y);

    void setScreenSize(int width, int height);

    void setPixelatedScreenSize(int width, int height);

    void isEnabled(bool value);

    void setFpsMode(bool fpsMode);

    [[nodiscard]] bool isFpsMode() const;

private:
    std::shared_ptr<jleWindow> windowInternal;

    int _lastMouseX{}, _lastMouseY{};
    int _deltaX{}, _deltaY;
    int _screenBeginX{0}, _screenBeginY{0};
    int _screenWidth, _screenHeight;
    int _pixelatedScreenWidth, _pixelatedScreenHeight;
    bool _isEnabled = true;
    bool _fpsMode{false};
};
