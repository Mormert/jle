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

#include "editor/jleEditorResourceRegistration.h"
void
jleEditorResourceRegistration::addResource(
    const std::vector<std::string> &fileEndings,
    const std::function<void(const jlePath &path, jleGameModules &gameModules)> &openResourcePromptFunction)
{
    for (const auto &fileEnding : fileEndings) {
        _openResourcePromptFunctions[fileEnding].push_back(openResourcePromptFunction);
    }
}
void
jleEditorResourceRegistration::invokeOpenResourcePromptFunctions(const jlePath &path, jleGameModules &gameModules) const
{
    if (const auto it = _openResourcePromptFunctions.find(path.getFileEnding()); it != _openResourcePromptFunctions.end()) {
        for (const auto &func : it->second) {
            func(path, gameModules);
        }
    }
}