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

#include <jlECS/jlECS.h>
#include "jleEditorImGuiWindowInterface.h"
#include "jleUndoRedo.h"
#include <modules/jleEditorUpdateContext.h>
#include <optional>

namespace jlECS
{
class ECS;
}

class jleECSEditorWindow : public jleEditorWindowInterface
{
public:
    explicit jleECSEditorWindow(const std::string &window_name);

    struct RenderUIInput{
        jleEditorUpdateContext &editorUpdate;
        jlECS::ECS& ecs;
    };

    struct RenderUIOutput{
        std::shared_ptr<std::vector<jlECS::ObjectRef>> selectedObjects = {};
    };

    RenderUIOutput renderUI(const RenderUIInput& input);

private:
    std::shared_ptr<std::vector<jlECS::ObjectRef>> _selectedObjects;
    std::vector<std::string> _deletionConfirmationList;
    int _lastSelectedIndex;
    jlECS::ECS *_ecs;

    jleUndoRedoManager _undoRedo{};
};
