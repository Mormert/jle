/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "jleMouseInput.h"
#include "jleProfiler.h"
#include "modules/windowing/jleWindow.h"

jleMouseInput::jleMouseInput(jleWindow &window) : _window{window} {}

int
jleMouseInput::mouseX()
{
    JLE_EXEC_IF(JLE_BUILD_EDITOR) { return _window.cursor().first - _screenBeginX; }
    else
    {
        return _window.cursor().first;
    }
}

int
jleMouseInput::mouseY()
{
    JLE_EXEC_IF(JLE_BUILD_EDITOR) { return _window.cursor().second - _screenBeginY; }
    else
    {
        return _window.cursor().second;
    }
}

float
jleMouseInput::xDelta() const
{
    return _deltaX;
}

float
jleMouseInput::yDelta() const
{
    return _deltaY;
}

float
jleMouseInput::scrollX()
{
    return _window.scrollX();
}

float
jleMouseInput::scrollY()
{
    return _window.scrollY();
}

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
jleMouseInput::setEnabled(bool value)
{
    _isEnabled = value;
}

bool
jleMouseInput::isEnabled()
{
    return _isEnabled;
}

bool
jleMouseInput::mouseClick(jleButton button)
{
    return mouseClick_int(static_cast<int>(button));
}

bool
jleMouseInput::mouseClick_int(int button)
{
    if (_isEnabled) {
        return _window.mouseClick(button);
    }
    return false;
}

void
jleMouseInput::setFpsMode(bool fpsMode)
{
    _fpsMode = fpsMode;
    _window.displayCursor(fpsMode);
}

bool
jleMouseInput::isFpsMode() const
{
    return _fpsMode;
}

void
jleMouseInput::updateDeltas()
{
    auto c = _window.cursor();
    _deltaX = c.first - _lastMouseX;
    _deltaY = c.second - _lastMouseY;
    _lastMouseX = c.first;
    _lastMouseY = c.second;
}
