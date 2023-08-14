// Copyright (c) 2023. Johan Lind

#include "jleMouseInput.h"
#include "jleProfiler.h"
#include "jleWindow.h"

jleMouseInput::jleMouseInput(std::shared_ptr<jleWindow> window) {
    this->_window = std::move(window);
}

int jleMouseInput::mouseX() {
#ifdef BUILD_EDITOR
    return _window->cursor().first - _screenBeginX;
#else
    return _window->cursor().first;
#endif
}

int jleMouseInput::mouseY() {
#ifdef BUILD_EDITOR
    return _window->cursor().second - _screenBeginY;
#else
    return _window->cursor().second;
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

float jleMouseInput::scrollX() { return _window->scrollX(); }

float jleMouseInput::scrollY() { return _window->scrollY(); }

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

void jleMouseInput::isEnabled(bool value) { _isEnabled = value; }

bool
jleMouseInput::mouseClick(jleButton button)
{
    if (_isEnabled) {
        return _window->mouseClick(static_cast<int>(button));
    }
    return false;
}

void
jleMouseInput::setFpsMode(bool fpsMode)
{
    _fpsMode = fpsMode;
    _window->displayCursor(fpsMode);
}

bool
jleMouseInput::isFpsMode() const
{
    return _fpsMode;
}
void
jleMouseInput::updateDeltas()
{
    JLE_SCOPE_PROFILE_CPU(jleMouseInput_updateDeltas)
    auto c = _window->cursor();
    _deltaX = c.first - _lastMouseX;
    _deltaY = c.second - _lastMouseY;
    _lastMouseX = c.first;
    _lastMouseY = c.second;
}
