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

#include "LinearMath/btMotionState.h"

class btCollisionShape;
class btRigidBody;
class cMesh;
class cTransform;
class jlePhysics;

namespace jlECS {
    class ECS;
}

class cRigidbodyMotionState final : public btMotionState {
public:
    cRigidbodyMotionState(jlECS::ECS *ecs, uint16_t objectIndex) : _ecs(ecs), _objectIndex(objectIndex){}

    void getWorldTransform(btTransform &worldTrans) const override;
    void setWorldTransform(const btTransform &worldTrans) override;
private:
    jlECS::ECS* _ecs;
    uint16_t _objectIndex;
};

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

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_mass), CEREAL_NVP(_shapeType));
    }

    btRigidBody &getBody();

    // Will update the rigidbody's shape to match the transforms scale
    void updateRigidbodyScaling() { _updateRigidbodyScaling = true; }

    bool isDynamic();

    enum class cRigidbodyShapeType {
        USE_MESH,
        CUBE,       // Not supported atm
        SPHERE,     // Not supported atm
    };

protected:
    friend class jlePhysics;
    friend class jlePhysicsModule;

    cRigidbodyShapeType _shapeType = cRigidbodyShapeType::USE_MESH;
    float _mass{0.f}; // Setting mass to 0 makes this rigidbody static.

    bool _updateRigidbodyScaling = false;

    btRigidBody* _body{nullptr};
    std::unique_ptr<btCollisionShape> _optionalLocalShape{nullptr};
    std::unique_ptr<cRigidbodyMotionState> _motionState{nullptr};
};
