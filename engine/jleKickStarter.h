// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleGameEngine.h"
#include "jleEditor.h"
#include "jleConfigUtils.h"
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>


namespace jle {

    template<typename T>
    void KickStartGame() {
        LOG_VERBOSE << "Kickstarting the game";
        static_assert(std::is_base_of<jleGame, T>::value, "T must derive from jleGame");

        auto gameSettings = std::make_shared<jleGameSettings>();

        cfg::LoadEngineConfig<jleGameSettings>(GAME_RESOURCES_DIRECTORY + "/jle_gs_config.json", *gameSettings);

        T::OverrideGameSettings(*gameSettings);

        auto gameEngine = std::make_unique<jleGameEngine>(gameSettings);
        gameEngine->SetGame<T>();
        gameEngine->Run();
    }

    template<typename T>
    void KickStartGameInEditor() {
        LOG_VERBOSE << "Kickstarting the editor";
        static_assert(std::is_base_of<jleGame, T>::value, "T must derive from jleGame");

        auto gameSettings = std::make_shared<jleGameSettings>();
        auto gameEditorSettings = std::make_shared<jleEditorSettings>();

        cfg::LoadEngineConfig<jleGameSettings>(GAME_RESOURCES_DIRECTORY + "/jle_gs_config.json", *gameSettings);
        cfg::LoadEngineConfig<jleEditorSettings>(GAME_RESOURCES_DIRECTORY + "/jle_es_config.json", *gameEditorSettings);

        T::OverrideGameSettings(*gameSettings);
        T::OverrideGameEditorSettings(*gameSettings, *gameEditorSettings);

        auto gameEngineInEditor = std::make_unique<jleEditor>(gameSettings, gameEditorSettings);
        gameEngineInEditor->SetGame<T>();
        gameEngineInEditor->Run();
    }

    template<typename T>
    void KickStart() {
        static_assert(std::is_base_of<jleGame, T>::value, "T must derive from jleGame");

        // Initialize plog when kickstarting, so logging is enabled everywhere after the kickstart
        static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("jle_log.plog", 1000,
                                                                          5);    // Log to txt files
        static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;                                                    // Log to command window
        plog::init<0>(plog::verbose, &fileAppender).addAppender(&consoleAppender);

#ifdef BUILD_EDITOR
        KickStartGameInEditor<T>();
#else
        KickStartGame<T>();
#endif
    }
}