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

#include "jlePhysics.h"

namespace jlECS
{
class ECS;
struct CreateCallbackData;
}

class cRigidbody;

class jlePhysicsModule
{
public:
    virtual void initializeECS(jlECS::ECS &ecs);

    struct UpdateContext {
        struct In {
            float dt = 1.f / 60.f;
        } in;

        struct InOut {
            jlECS::ECS &ecs;
            jlePhysics& physics;
        } inOut;

        struct Out {
        } out;
    };

    void update(UpdateContext &ctx);

protected:
    void onRigidbodyCreated(jlECS::CreateCallbackData &createCallbackData);
    void onRigidbodyDestroyed(cRigidbody* rb);
    void onRigidbodyCopied(cRigidbody *source, cRigidbody *dest);

    jlePhysics* _physics;
};
