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

#include "jleBuildConfig.h"

#include <memory>

class jleWindow;

enum class jleButton : int
{
    BUTTON_1        = 0,
    BUTTON_2        = 1,
    BUTTON_3        = 2,
    BUTTON_4        = 3,
    BUTTON_5        = 4,
    BUTTON_6        = 5,
    BUTTON_7        = 6,
    BUTTON_8        = 7,
    BUTTON_LAST     = BUTTON_8,
    BUTTON_LEFT     = BUTTON_1,
    BUTTON_RIGHT    = BUTTON_2,
    BUTTON_MIDDLE   = BUTTON_3
};

class jleMouseInput
{
public:
    explicit jleMouseInput(jleWindow& window);

    void updateDeltas();

    int mouseX();

    int mouseY();

    float xDelta() const;

    float yDelta() const;

    float scrollX();

    float scrollY();

    bool mouseClick(jleButton button);

    void setScreenBeginCoords(int x, int y);

    void setScreenSize(int width, int height);

    void isEnabled(bool value);

    void setFpsMode(bool fpsMode);

    [[nodiscard]] bool isFpsMode() const;

private:
    jleWindow& _window;

    int _lastMouseX{}, _lastMouseY{};
    int _deltaX{}, _deltaY{};
    int _screenBeginX{0}, _screenBeginY{0};
    int _screenWidth{}, _screenHeight{};
    bool _isEnabled = true;
    bool _fpsMode{false};
};
