// Copyright (c) 2023. Johan Lind

#pragma once

#include "editor/jleEditor.h"
#include "jleGameEngine.h"
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>

template <typename T>
void kickStartGame() {
    LOG_VERBOSE << "Kickstarting the game";
    static_assert(std::is_base_of<jleGame, T>::value,
                  "T must derive from jleGame");

    auto gameEngine = std::make_unique<jleGameEngine>();
    gameEngine->setGame<T>();
    gameEngine->run();
}

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
    plog::init<0>(plog::verbose, &fileAppender).addAppender(&consoleAppender);

#ifdef BUILD_EDITOR
    kickStartGameInEditor<T>();
#else
    kickStartGame<T>();
#endif
}
