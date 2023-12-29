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

#include "editor/jleEditor.h"
#include "jleGameEngine.h"
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include "jleDynamicLogAppender.h"

template <typename T>
void kickStartGame() {
    LOG_VERBOSE << "Kickstarting the game";
    static_assert(std::is_base_of<jleGame, T>::value,
                  "T must derive from jleGame");

    auto gameEngine = std::make_unique<jleGameEngine>();
    gameEngine->setGame<T>();
    gameEngine->run();
}

#ifdef BUILD_EDITOR
template <typename T>
void kickStartGameInEditor() {
    LOG_VERBOSE << "Kickstarting the editor";
    static_assert(std::is_base_of<jleGame, T>::value,
                  "T must derive from jleGame");

    auto gameEngineInEditor =
        std::make_unique<jleEditor>();
    gameEngineInEditor->setGame<T>();
    gameEngineInEditor->run();
}
#endif // BUILD_EDITOR

template <typename T>
void kickStart() {
    static_assert(std::is_base_of<jleGame, T>::value,
                  "T must derive from jleGame");


    // Initialize plog when kickstarting, so logging is enabled everywhere after
    // the kickstart
    plog::RollingFileAppender<plog::TxtFormatter> fileAppender(
        "jle_log.plog", 100000, 5);
    plog::ColorConsoleAppender<plog::TxtFormatter>
        consoleAppender; // Log to command window
    plog::init<0>(plog::verbose, &fileAppender).addAppender(&consoleAppender).addAppender(&dynamicAppender());

#ifdef BUILD_EDITOR
    kickStartGameInEditor<T>();
#else
    kickStartGame<T>();
#endif
}
