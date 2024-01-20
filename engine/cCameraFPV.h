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

#include "cCamera.h"
#include "jleCamera.h"

class cCameraFPV : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cCameraFPV)
public:

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_moveSpeed), CEREAL_NVP(_mouseSensitivity));
    }

    ~cCameraFPV() override;

    void update(float dt) override;

protected:
    float _moveSpeed{100.f};
    float _mouseSensitivity{1.f};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cCameraFPV)

CEREAL_REGISTER_TYPE(cCameraFPV)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cCameraFPV)
