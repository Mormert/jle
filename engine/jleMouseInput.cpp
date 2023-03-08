// Copyright (c) 2023. Johan Lind

#include "jleMouseInput.h"

#include "jleWindow.h"
#include <GLFW/glfw3.h>
#include <utility>

jleMouseInput::jleMouseInput(std::shared_ptr<jleWindow> windowInternal) {
    this->windowInternal = std::move(windowInternal);
}

void jleMouseInput::linkWindow(std::shared_ptr<jleWindow> windowInternal) {
    this->windowInternal = windowInternal;
}

int jleMouseInput::mouseX() {
#ifdef BUILD_EDITOR
    return windowInternal->cursor().first - _screenBeginX;
#else
    return windowInternal->cursor().first;
#endif
}

int jleMouseInput::mouseY() {
#ifdef BUILD_EDITOR
    return windowInternal->cursor().second - _screenBeginY;
#else
    return windowInternal->cursor().second;
#endif
}

float
jleMouseInput::xDelta()
{
    return _deltaX;
}

float
jleMouseInput::yDelta()
{
    return _deltaY;
}

float jleMouseInput::scrollX() { return windowInternal->scrollX(); }

float jleMouseInput::scrollY() { return windowInternal->scrollY(); }

void
jleMouseInput::setScreenBeginCoords(int x, int y)
{
    _screenBeginX = x;
    _screenBeginY = y;
}

void
jleMouseInput::setScreenSize(int width, int height)
{
    _screenWidth = width;
    _screenHeight = height;
}

void
jleMouseInput::setPixelatedScreenSize(int width, int height)
{
    _pixelatedScreenWidth = width;
    _pixelatedScreenHeight = height;
}

void jleMouseInput::isEnabled(bool value) { _isEnabled = value; }

int jleMouseInput::pixelatedMouseX() {
    const float ratio = float(_pixelatedScreenWidth) / float(_screenWidth);
    return int(ratio * float(mouseX()));
}

int jleMouseInput::pixelatedMouseY() {
    const float ratio = float(_pixelatedScreenHeight) / float(_screenHeight);
    return int(ratio * float(mouseY()));
}

bool
jleMouseInput::mouseClick(int button)
{
    if (_isEnabled) {
        return windowInternal->mouseClick(button);
    }
    return false;
}

void
jleMouseInput::setFpsMode(bool fpsMode)
{
    _fpsMode = fpsMode;
    windowInternal->displayCursor(fpsMode);
}

bool
jleMouseInput::isFpsMode() const
{
    return _fpsMode;
}
void
jleMouseInput::updateDeltas()
{
    auto c = windowInternal->cursor();
    _deltaX = c.first - _lastMouseX;
    _deltaY = c.second - _lastMouseY;
    _lastMouseX = c.first;
    _lastMouseY = c.second;
}
