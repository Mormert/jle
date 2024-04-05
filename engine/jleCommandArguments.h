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

#include "jleCommon.h"

#include <string>
#include <vector>

class jleCommandArguments
{
public:
    jleCommandArguments(const jleCommandArguments&) = delete;
    jleCommandArguments& operator=(const jleCommandArguments&) = delete;

    void parse(int argc, char *argv[]);

    [[nodiscard]] bool hasArgument(const std::string &arg) const;

    const std::string& getProgramArgsString();

    static jleCommandArguments& getInstance();

private:
    jleCommandArguments() = default;

    std::vector<std::string> _arguments{};
    std::string _programArgsString{};
};