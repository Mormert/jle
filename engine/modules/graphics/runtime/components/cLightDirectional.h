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

#include "core/jleCommon.h"

#include "core/serialization/jleExternalSerialization.h"

#include <glm/vec3.hpp>


class jleFramePacket;
class cTransform;

namespace sol
{
class state;
}

class cLightDirectional
{
public:
    template <class Archive>
    void serialize(Archive &ar){
        ar(CEREAL_NVP(_color));
    }

    void ecsUpdate(jleFramePacket &packet, const cTransform& transform);

    static void registerLua(sol::state &lua);

    //void editorGizmosRender(jleFramePacket &renderGraph, jleEditorGizmos &gizmos) override;

protected:
    glm::vec3 _color{1.f};
};

