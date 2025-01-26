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

#include "modules/input/jleInputModule.h"

class jleInputModuleEditor : public jleInputModuleBase
{
public:
    jleInputModuleEditor() = default;

    bool isKeyPressed(jleKey key) override;
    bool isKeyReleased(jleKey key) override;
    bool isKeyDown(jleKey key) override;

    int32_t getMouseX() override;
    int32_t getMouseY() override;

    float getScrollX() override;
    float getScrollY() override;

    bool getMouseClick(jleButton button) override;

    void setScreenBeginCoords(uint32_t x, uint32_t y) { _screenBeginX = x; _screenBeginY = y; }

    void setGameWindowFocused(bool focused) { _gameWindowFocused = focused; }
    [[nodiscard]] bool isGameWindowFocused() const { return _gameWindowFocused; }

private:
    int32_t _screenBeginX{0}, _screenBeginY{0};
    bool _gameWindowFocused{false};

    static int jleKeyToImGuiKey(jleKey key);
    static int jleButtonToImGuiButton(jleButton button);
};