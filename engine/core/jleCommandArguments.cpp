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

#include "jleCommandArguments.h"

void jleCommandArguments::parse(int argc, char **argv)
{
    _arguments.resize(argc - 1);
    for (int i = 1; i < argc; ++i) {
        _arguments[i - 1] = argv[i];
        _programArgsString += argv[i];
        if (i < argc - 1) {
            _programArgsString += " ";
        }
    }
}

bool
jleCommandArguments::hasArgument(const std::string &arg) const
{
    for (const std::string &argument : _arguments) {
        if (argument == arg) {
            return true;
        }
    }
    return false;
}

const std::string &
jleCommandArguments::getProgramArgsString()
{
    return _programArgsString;
}

jleCommandArguments &
jleCommandArguments::getInstance()
{
    static jleCommandArguments args;
    return args;
}
