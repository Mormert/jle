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

#include <memory>
#include <ranges>
#include <vector>

struct jleSerializationContext;

class jleUndoRedoCommandBase {
public:
    virtual ~jleUndoRedoCommandBase() = default;

    struct CommandContext{
        jleSerializationContext& serializationContext;
    };

    virtual void execute(const CommandContext& ctx) = 0;
    virtual void undo(const CommandContext& ctx) = 0;
};

class jleChainedUndoRedoCommand : public jleUndoRedoCommandBase{
public:
    explicit jleChainedUndoRedoCommand(std::vector<std::unique_ptr<jleUndoRedoCommandBase>>& chainedCommands)
        : _chainedCommands(std::move(chainedCommands)){}

    ~jleChainedUndoRedoCommand() override = default;

    void execute(const CommandContext& ctx) override{
        for (const auto& command : _chainedCommands) {
            command->execute(ctx);
        }
    }

    void undo(const CommandContext& ctx) override{
        for (const auto &command : std::ranges::reverse_view(_chainedCommands)) {
            command->undo(ctx);
        }
    }
private:
    std::vector<std::unique_ptr<jleUndoRedoCommandBase>> _chainedCommands;
};

class jleUndoRedoManager {
public:
    void enqueueAndExecute(const jleUndoRedoCommandBase::CommandContext& ctx, std::unique_ptr<jleUndoRedoCommandBase> command);
    void enqueue(std::unique_ptr<jleUndoRedoCommandBase> command);

    void undo(const jleUndoRedoCommandBase::CommandContext& ctx);
    void redo(const jleUndoRedoCommandBase::CommandContext& ctx);
    void clear();

private:
    std::vector<std::unique_ptr<jleUndoRedoCommandBase>> _commands;
    int _currentCommandIndex = 0; // Points to the next command to execute
};
