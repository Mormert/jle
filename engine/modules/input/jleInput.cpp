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


#include "jleInput.h"
#include "modules/input/hardware/jleKeyboardInput.h"
#include "modules/input/hardware/jleMouseInput.h"

jleInput::jleInput(jleWindow& window)
    : keyboard{std::make_unique<jleKeyboardInput>(window)}, mouse{std::make_unique<jleMouseInput>(window)}
{
}

void
jleInput::setInputEnabled(bool isEnabled)
{
    _isInputEnabled = isEnabled;
    keyboard->isEnabled(isEnabled);
    mouse->isEnabled(isEnabled);
}

bool
jleInput::getInputEnabled() const
{
    return _isInputEnabled;
}
