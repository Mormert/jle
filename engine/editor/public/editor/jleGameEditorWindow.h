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

#include "editor/jleEditorImGuiWindowInterface.h"

class jleEditorWindow;
struct jleEngineUpdateContext;

class jleGameEditorWindow : public jleEditorWindowInterface
{
public:
    explicit jleGameEditorWindow(const std::string &window_name);

    void renderUI(jleEngineUpdateContext &ctx);

    int width() const;

    int height() const;

    int x() const;

    int y() const;

private:
    uint32_t _lastGameWindowWidth = 0.f, _lastGameWindowHeight = 0.f;
    uint32_t _windowPositionX = 0, _windowPositionY = 0;
    bool _wasFocused = false;
};