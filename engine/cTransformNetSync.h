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

#include "jleComponent.h"

class cTransformNetSync: public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cTransformNetSync)
public:
    void netSyncOut(cereal::BinaryOutputArchive &ar) override;

    void netSyncIn(cereal::BinaryInputArchive &ar) override;

    void serverUpdate(float dt) override;

protected:
    glm::vec3 _color{1.f};
};

CEREAL_REGISTER_TYPE(cTransformNetSync)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cTransformNetSync)

