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
#include "modules/input/hardware/jleKeyboardInput.h"
#include "modules/input/hardware/jleMouseInput.h"

jleInputModule::jleInputModule(jleWindow& window)
    : keyboard{window}, mouse{window}
{
}

void
jleInputModule::setInputEnabled(bool isEnabled)
{
    _isInputEnabled = isEnabled;
    keyboard.setEnabled(isEnabled);
    mouse.setEnabled(isEnabled);
}

bool
jleInputModule::getInputEnabled() const
{
    return _isInputEnabled;
}
