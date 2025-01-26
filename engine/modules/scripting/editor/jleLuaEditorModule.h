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

#include "modules/scripting/jleLuaModule.h"
#include <modules/jleEditorUpdateContext.h>

namespace jlECS
{
class ComponentContainer;
}
class jleImGuiArchive;

class jleLuaEditorModule final : public jleLuaModule
{
public:
    explicit jleLuaEditorModule(bool gameRunning) : _gameRunning{gameRunning} {}

    void initializeECS(jlECS::ECS &ecs) override;
    void initializeModule(jleSerializationContext &serializationContext) override;

    void updateEditor(jleSerializationContext &serializationContext) const;

private:
    static void editorSerializeLuaScript(jlECS::ComponentContainer *cc, jleImGuiArchive& archive, int componentIndex, int objectIndex);

    bool _gameRunning;
};
