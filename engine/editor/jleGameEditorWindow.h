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

#ifdef BUILD_EDITOR

#include "editor/jleEditorImGuiWindowInterface.h"

#include "jleGameEngine.h"

class jleGameEditorWindow : public jleEditorWindowInterface
{
public:
    explicit jleGameEditorWindow(const std::string &window_name);

    void update(jleGameEngine &ge) override;

    static inline jleGameEditorWindow *gGameEditorWindow;

    int width() const;

    int height() const;

    int x() const;

    int y() const;

private:
    float _lastGameWindowWidth = 0.f, _lastGameWindowHeight = 0.f;
    int _windowPositionX, _windowPositionY;
    bool _wasFocused = false;
};

#endif // BUILD_EDITOR
