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

#include "editor/jleEditor.h"
#include "jleCommandArguments.h"
#include "jleDynamicLogAppender.h"
#include "jleGameEngine.h"
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>

class jleKickStarter
{
public:
    template <typename T>
    void
    kickStartGame()
    {
        JLE_EXEC_IF(JLE_BUILD_HEADLESS) { LOG_VERBOSE << "Kickstarting the game in headless build (no graphics)"; }
        else
        {
            LOG_VERBOSE << "Kickstarting the game";
        }
        static_assert(std::is_base_of<jleGame, T>::value, "T must derive from jleGame");

        auto gameEngine = std::make_unique<jleGameEngine>();
        gameEngine->setGame<T>();
        gameEngine->run();
    }

#if JLE_BUILD_EDITOR
    template <typename T>
    void
    kickStartGameInEditor()
    {
        LOG_VERBOSE << "Kickstarting the editor";
        static_assert(std::is_base_of<jleGame, T>::value, "T must derive from jleGame");

        auto gameEngineInEditor = std::make_unique<jleEditor>();
        gameEngineInEditor->setGame<T>();
        gameEngineInEditor->run();
    }
#endif // JLE_BUILD_EDITOR

    template <typename T>
    void
    kickStart(int argc, char *argv[])
    {
        static_assert(std::is_base_of<jleGame, T>::value, "T must derive from jleGame");

#if JLE_BUILD_RUNTIME_CONFIGURABLE
        jleCommandArguments arguments{argc, argv};
        configureRuntime(arguments);
#endif

        // Initialize plog when kickstarting, so logging is enabled everywhere after the kickstart
        plog::RollingFileAppender<plog::TxtFormatter> fileAppender("jle_log.plog", 100000, 5);
        plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender; // Log to command window
        plog::init<0>(plog::verbose, &fileAppender).addAppender(&consoleAppender).addAppender(&dynamicAppender());

        JLE_EXEC_IF(JLE_BUILD_EDITOR)
        {
#if JLE_BUILD_EDITOR
            kickStartGameInEditor<T>();
#endif
        }
        else
        {
            kickStartGame<T>();
        }
    }

protected:
#if JLE_BUILD_RUNTIME_CONFIGURABLE
    virtual void
    configureRuntime(const jleCommandArguments &commandLineArguments)
    {
        if (commandLineArguments.hasArgument("-game")) {
            JLE_BUILD_EDITOR_RUNTIME = false;
        }
        if (commandLineArguments.hasArgument("-headless")) {
            JLE_BUILD_HEADLESS_RUNTIME = true;
        }
    }
#endif
};
