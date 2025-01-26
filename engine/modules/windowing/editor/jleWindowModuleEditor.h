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

#include "modules/windowing/jleWindowModule.h"

class jleEditorWindow;

class jleWindowModuleEditor final : public jleWindowModuleBase
{
public:
    uint32_t getWindowWidth() override { return _windowWidth; }
    uint32_t getWindowHeight() override { return _windowHeight; }

    void setWindowWidth(uint32_t width) { _windowWidth = width; }
    void setWindowHeight(uint32_t height) { _windowHeight = height; }
private:
    uint32_t _windowWidth{0};
    uint32_t _windowHeight{0};
};