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

#include "core/jleCommon.h"

#include "modules/input/hardware/jleKeyboardInput.h"
#include "modules/input/hardware/jleMouseInput.h"
#include <memory>

class jleWindow;

class jleInput
{
public:
    ~jleInput() = default;

    explicit jleInput(jleWindow& window);

    const std::unique_ptr<jleKeyboardInput> keyboard;
    const std::unique_ptr<jleMouseInput> mouse;

    void setInputEnabled(bool isEnabled);

    [[nodiscard]] bool getInputEnabled() const;

private:
    bool _isInputEnabled = true;
};