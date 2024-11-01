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

class jleFramePacket;
class jleCamera;

namespace jlECS
{
class ECS;
}

class jleGraphicsModule
{
public:
    void initializeECS(jlECS::ECS &ecs);

    struct UpdateContext {
        struct In {
            int screenX;
            int screenY;
        } in;

        struct InOut {
            jlECS::ECS &ecs;
        } inOut;

        struct Out {
            jleFramePacket &framePacket;
            jleCamera &camera;
        } out;
    };

    void update(jleGraphicsModule::UpdateContext &ctx);
};
