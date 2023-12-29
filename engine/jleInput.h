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

#pragma once

#include "jleKeyboardInput.h"
#include "jleMouseInput.h"
#include <memory>

class jleWindow;

class jleInput
{
public:
    ~jleInput() = default;

    explicit jleInput(const std::shared_ptr<jleWindow> &window);

    const std::unique_ptr<jleKeyboardInput> keyboard;
    const std::unique_ptr<jleMouseInput> mouse;

    void setInputEnabled(bool isEnabled);

    bool getInputEnabled() const;

private:
    bool _isInputEnabled = true;
};
