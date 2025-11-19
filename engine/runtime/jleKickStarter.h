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

#include "core/jleCommon.h"
#include "core/jleCommandArguments.h"
#include "core/jleDynamicLogAppender.h"

#include "jleGameEngine.h"

#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>

class jleKickStarter
{
public:
    void kickStart(std::unique_ptr<jleGameEngine> engine, int argc, char *argv[])
    {
#if JLE_BUILD_RUNTIME_CONFIGURABLE
        jleCommandArguments commandArguments{argc, argv};
        configureRuntime(commandArguments);
#endif
        // Initialize plog when kickstarting, so logging is enabled everywhere after the kickstart
        plog::RollingFileAppender<plog::TxtFormatter> fileAppender("jle_log.plog", 1000000, 5);
        plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender; // Log to command window
        plog::init<0>(plog::verbose, &fileAppender).addAppender(&consoleAppender).addAppender(&dynamicAppender());

        engine->run();
        engine.reset();
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
