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

#include "jlECS/jlECS.h"
#include "core/serialization/jleBinaryArchive.h"
#include "core/serialization/jleJSONArchive.h"
#include "editor/jleImGuiArchive.h"

template <typename T> void registerEditorECSComponent(jlECS::ECS& ecs){
    jlECS::ComponentRegistrationConfig config{
        .serializeInputF_JSON = jlECS::Serialization::serializeInputT_JSON<T>,
        .serializeOutputF_JSON = jlECS::Serialization::serializeOutputT_JSON<T>,
        .serializeInputF_Binary = jlECS::Serialization::serializeInputT_Binary<T>,
        .serializeOutputF_Binary = jlECS::Serialization::serializeOutputT_Binary<T>,
        .serializeImGuiF = jlECS::Serialization::serialize_ImGui<T>
    };

    ecs.registerComponentType<T>(config);
}