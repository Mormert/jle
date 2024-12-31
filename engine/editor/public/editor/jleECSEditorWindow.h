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

#include <jleUndoRedo.h>

#include "jleBuildConfig.h"

#include <jlECS/jlECS.h>
#include "jleEditorImGuiWindowInterface.h"
#include <modules/jleEditorUpdateContext.h>
#include <optional>

namespace jlECS
{
class ECS;
}

class jleUndoRedoManager;


class jleECSEditorWindow : public jleEditorWindowInterface
{
public:
    explicit jleECSEditorWindow(const std::string &window_name);

    struct RenderUIInput{
        jleEditorUpdateContext &editorUpdate;
        jlECS::ECS& ecs;
        jleUndoRedoManager& undoRedo;
    };

    struct RenderUIOutput{
        std::shared_ptr<std::vector<jlECS::ObjectRef>> selectedObjects = {};
    };

    RenderUIOutput renderUI(const RenderUIInput& input);

private:
    void handleUndoRedoShortcuts(const RenderUIInput &input,
                                 jleUndoRedoCommandBase::CommandContext &undoRedoCommandCtx);

    void handleSaveLoadButtons(const RenderUIInput &input, jleUndoRedoCommandBase::CommandContext &undoRedoCommandCtx,
                               jlECS::ECS &ecs, jleSerializationContext &serializationContext);

    void handleObjectHierarchy(const RenderUIInput &input, jleUndoRedoCommandBase::CommandContext &undoRedoCommandCtx,
                               jlECS::ECS &ecs, jleSerializationContext &serializationContext);

    void handleSelectedObjectsPane(const RenderUIInput &input,
                                   jleUndoRedoCommandBase::CommandContext &undoRedoCommandCtx,
                                   jlECS::ECS &ecs);

    void handleDeletionConfirmation(const RenderUIInput &input,
                                    jleUndoRedoCommandBase::CommandContext &undoRedoCommandCtx,
                                    jlECS::ECS &ecs);

    std::shared_ptr<std::vector<jlECS::ObjectRef>> _selectedObjects;
    std::vector<std::string> _deletionConfirmationList;
    int _lastSelectedIndex;
    int _expandParentIndex = -1;
    jlECS::ECS *_ecs;
};
