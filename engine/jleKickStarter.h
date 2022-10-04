// Copyright (c) 2022. Johan Lind

#pragma once

#include "editor/jleEditor.h"
#include "jleConfigUtils.h"
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

    auto gameSettings = std::make_shared<jleGameSettings>();

    cfg::loadEngineConfig<jleGameSettings>(
        GAME_RESOURCES_DIRECTORY + "/jle_gs_config.json", *gameSettings);

    T::overrideGameSettings(*gameSettings);

    auto gameEngine = std::make_unique<jleGameEngine>(gameSettings);
    gameEngine->SetGame<T>();
    gameEngine->run();
}

template <typename T>
void kickStartGameInEditor() {
    LOG_VERBOSE << "Kickstarting the editor";
    static_assert(std::is_base_of<jleGame, T>::value,
                  "T must derive from jleGame");

    auto gameSettings = std::make_shared<jleGameSettings>();
    auto gameEditorSettings = std::make_shared<jleEditorSettings>();

    cfg::loadEngineConfig<jleGameSettings>(
        GAME_RESOURCES_DIRECTORY + "/jle_gs_config.json", *gameSettings);
    cfg::loadEngineConfig<jleEditorSettings>(
        GAME_RESOURCES_DIRECTORY + "/jle_es_config.json", *gameEditorSettings);

    T::overrideGameSettings(*gameSettings);
    T::overrideGameEditorSettings(*gameSettings, *gameEditorSettings);

    auto gameEngineInEditor =
        std::make_unique<jleEditor>(gameSettings, gameEditorSettings);
    gameEngineInEditor->SetGame<T>();
    gameEngineInEditor->run();
}

template <typename T>
void kickStart() {
    static_assert(std::is_base_of<jleGame, T>::value,
                  "T must derive from jleGame");

    // Initialize plog when kickstarting, so logging is enabled everywhere after
    // the kickstart
    plog::RollingFileAppender<plog::TxtFormatter> fileAppender(
        "jle_log.plog", 1000, 5);
    plog::ColorConsoleAppender<plog::TxtFormatter>
        consoleAppender; // Log to command window
    plog::init<0>(plog::verbose, &fileAppender).addAppender(&consoleAppender);

#ifdef BUILD_EDITOR
    kickStartGameInEditor<T>();
#else
    kickStartGame<T>();
#endif
}