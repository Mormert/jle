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
#include "modules/graphics/jleMaterial.h"
#include "modules/graphics/jleMesh.h"

#include "core/jleResourceRef.h"


#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btMotionState.h>

class btRigidBody;
class cMesh;
class cTransform;
class jlePhysics;

namespace jlECS {
    class ECS;
}

class cRigidbody
{
public:
    cRigidbody();
    ~cRigidbody();

    // Move Constructor & Assignment
    cRigidbody(cRigidbody&& other) noexcept;
    cRigidbody& operator=(cRigidbody&& other) noexcept;

    // Delete Copy Operations
    cRigidbody(const cRigidbody&) = delete;
    cRigidbody& operator=(const cRigidbody&) = delete;

    static void duplicate(jlePhysics* physics, cRigidbody* source, cRigidbody* dest);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_mass));
    }

    btRigidBody &getBody();

    void setWorldMatrixAndScaleRigidbody(jlePhysics* physics, cTransform& transform, cMesh& mesh);

    bool isDynamic();

protected:
    friend class jlePhysics;
    friend class jlePhysicsModule;

    void setupRigidbody(jlePhysics* physics, cTransform& transform, cMesh& mesh);

    std::unique_ptr<btRigidBody> createRigidbody(bool isDynamic, const cTransform& transform, btCollisionShape *shape);

    float _mass{0.f}; // Setting mass to 0 makes this rigidbody static.
    glm::vec3 _size{1.f};

    std::unique_ptr<btRigidBody> _body{nullptr};
    std::unique_ptr<btCollisionShape> _optionalLocalShape{nullptr};
};
