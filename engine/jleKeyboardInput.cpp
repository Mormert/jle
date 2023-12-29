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

#include "jleKeyboardInput.h"
#include "jleWindow.h"

#include <utility>

jleKeyboardInput::jleKeyboardInput(std::shared_ptr<jleWindow> window) {
    this->_window = std::move(window);
}

bool jleKeyboardInput::keyPressed(jleKey key) {
    if (_isEnabled) {
        return _window->keyPressed(static_cast<int>(key));
    }
    return false;
}

bool jleKeyboardInput::keyReleased(jleKey key) {
    if (_isEnabled) {
        return _window->keyReleased(static_cast<int>(key));
    }
    return false;
}

bool jleKeyboardInput::keyDown(jleKey key, bool ignoreInputEnabled) {
    if (ignoreInputEnabled) {
        return _window->key(static_cast<int>(key));
    }
    if (_isEnabled) {
        return _window->key(static_cast<int>(key));
    }
    return false;
}

void jleKeyboardInput::isEnabled(bool value) { _isEnabled = value; }
