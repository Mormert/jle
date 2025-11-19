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


#include "jleUndoRedo.h"

void jleUndoRedoManager::enqueueAndExecute(const jleUndoRedoCommandBase::CommandContext& ctx, std::unique_ptr<jleUndoRedoCommandBase> command) {
    // If there are any commands ahead of the current index, remove them
    if (_currentCommandIndex < static_cast<int>(_commands.size())) {
        _commands.erase(_commands.begin() + _currentCommandIndex, _commands.end());
    }

    command->execute(ctx);

    _commands.emplace_back(std::move(command));
    _currentCommandIndex++;
}

void
jleUndoRedoManager::enqueue(std::unique_ptr<jleUndoRedoCommandBase> command)
{
    if (_currentCommandIndex < static_cast<int>(_commands.size())) {
        _commands.erase(_commands.begin() + _currentCommandIndex, _commands.end());
    }

    _commands.emplace_back(std::move(command));
    _currentCommandIndex++;
}

void jleUndoRedoManager::undo(const jleUndoRedoCommandBase::CommandContext& ctx) {
    if (_currentCommandIndex == 0) {
        return;
    }

    _currentCommandIndex--;
    _commands[_currentCommandIndex]->undo(ctx);
}

void jleUndoRedoManager::redo(const jleUndoRedoCommandBase::CommandContext& ctx) {
    if (_currentCommandIndex >= static_cast<int>(_commands.size())) {
        return;
    }

    _commands[_currentCommandIndex]->execute(ctx);
    _currentCommandIndex++;
}

void jleUndoRedoManager::clear() {
    _commands.clear();
    _currentCommandIndex = 0;
}

