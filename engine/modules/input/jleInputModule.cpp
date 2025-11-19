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


#include "jleInputModule.h"
#include "modules/windowing/jleWindowModule.h"

void
jleInputModuleBase::update()
{
    const int32_t currentX = getMouseX();
    const int32_t currentY = getMouseY();
    _deltaX = currentX - _lastMouseX;
    _deltaY = currentY - _lastMouseY;
    _lastMouseX = currentX;
    _lastMouseY = currentY;
}

jleGameInputModule::jleGameInputModule(jleWindow& window) : _window(window) { }

bool
jleGameInputModule::isKeyPressed(jleKey key)
{
    if (!_isInputEnabled) {
        return false;
    }
    return _window.getKeyPressed(static_cast<int>(key));
}

bool
jleGameInputModule::isKeyReleased(jleKey key)
{
    if (!_isInputEnabled) {
        return false;
    }
    return _window.getKeyReleased(static_cast<int>(key));
}

bool
jleGameInputModule::isKeyDown(jleKey key)
{
    if (!_isInputEnabled) {
        return false;
    }
    return _window.getKeyDown(static_cast<int>(key));
}

int32_t
jleGameInputModule::getMouseX()
{
    return _window.getCursor().first;
}

int32_t
jleGameInputModule::getMouseY()
{
    return _window.getCursor().second;
}

float
jleGameInputModule::getScrollX()
{
    return _window.getScrollX();
}

float
jleGameInputModule::getScrollY()
{
    return _window.getScrollY();
}

bool
jleGameInputModule::getMouseClick(jleButton button)
{
    if (!_isInputEnabled) {
        return false;
    }
    return _window.getMouseClick(static_cast<int>(button));
}
