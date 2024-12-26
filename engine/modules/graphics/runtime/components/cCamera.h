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

#include <cereal/cereal.hpp>

class cTransform;
class jleCamera;

class cCamera
{
public:
    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(perspective),
           CEREAL_NVP(farPlane),
           CEREAL_NVP(nearPlane),
           CEREAL_NVP(perspectiveFov),
           CEREAL_NVP(framebufferSizeX),
           CEREAL_NVP(framebufferSizeY));
    }

    struct UpdateContext{
        struct In{
            const cTransform& transform;
            uint32_t width;
            uint32_t height;
        } in;

        struct Out{
            jleCamera& camera;
        } out;
    };

    void update(UpdateContext& ctx) const;

    bool perspective{true};
    float perspectiveFov{90.f};
    float farPlane{10000.f};
    float nearPlane{0.1f};
    int framebufferSizeX{1024};
    int framebufferSizeY{1024};
};