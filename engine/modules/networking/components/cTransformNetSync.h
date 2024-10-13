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

#include "core/jleComponent.h"

class cTransformNetSync: public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cTransformNetSync)
public:
    void netSyncOut(jleBinaryOutputArchive &ar) override;

    void netSyncIn(jleBinaryInputArchive &ar) override;

    void serverUpdate(jleEngineModulesContext& ctx) override;

protected:
    glm::vec3 _color{1.f};
};

CEREAL_REGISTER_TYPE(cTransformNetSync)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cTransformNetSync)

