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

#include "core/jlePath.h"

#include <functional>
#include <unordered_map>

class jlePath;
struct jleGameModules;

class jleEditorResourceRegistration
{
public:
    void addResource(
        const std::vector<std::string> &fileEndings,
        const std::function<void(const jlePath &path, jleGameModules &gameModules)> &openResourcePromptFunction);

    void invokeOpenResourcePromptFunctions(const jlePath &path, jleGameModules &gameModules) const;

private:
    std::unordered_map<std::string, std::vector<std::function<void(const jlePath& path, jleGameModules& gameModules)>>> _openResourcePromptFunctions;
};
