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

#include <cstdint>

class jleFramePacket;
class jleCamera;

namespace jlECS
{
class ECS;
}

class jleGraphicsModule
{
public:
    virtual void initializeECS(jlECS::ECS &ecs);

    struct UpdateContext {
        struct In {
            uint32_t screenX;
            uint32_t screenY;
        } in;

        struct InOut {
            jlECS::ECS &ecs;
        } inOut;

        struct Out {
            jleFramePacket &framePacket;
        } out;
    };

    void update(UpdateContext &ctx);
};
