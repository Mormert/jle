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

#ifndef JLE_PHYSICS_H
#define JLE_PHYSICS_H

#include "core/jleCommon.h"

#include <memory>

class btRigidBody;
class btDiscreteDynamicsWorld;

class cRigidbody;
class jleFramePacket;

class jlePhysics
{
public:
    jlePhysics();
    ~jlePhysics();

    void step(float dt);

    void addRigidbody(btRigidBody *body);

    void removeRigidbody(btRigidBody *body);

    bool renderDebugEnabled = false;

    void renderDebug(jleFramePacket &packet);

    btDiscreteDynamicsWorld &dynamicsWorld();
private:
    struct jlePhysicsInternal;
    std::unique_ptr<jlePhysicsInternal> _internal;
};

#endif // JLE_PHYSICS_H
