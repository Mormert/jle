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
#include "core/jleTransform.h"
#include "modules/graphics/jleMaterial.h"
#include "modules/graphics/jleMesh.h"

#include "core/jleResourceRef.h"


#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btMotionState.h>

class btRigidBody;

class cRigidbody : public jleComponent, public btMotionState
{
    JLE_REGISTER_COMPONENT_TYPE(cRigidbody)
public:
    cRigidbody();

    ~cRigidbody();

    // Called when cloned/duplicated
    cRigidbody(const cRigidbody &other);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_mass));
    }

    void editorUpdate(jleEngineUpdateContext & ctx) override;

    void start(jleEngineUpdateContext & ctx) override;

    void update(jleEngineUpdateContext & ctx) override;

    void onDestroy(jleEngineUpdateContext & ctx) override;

    btRigidBody &getBody();

    void setWorldMatrixAndScaleRigidbody(const glm::mat4& worldMatrix);

    void setupRigidbody();

    void setupNewRigidbodyAndDeleteOld();

    bool isDynamic();

protected:
    friend class jlePhysics;

    // Sync object transform to bullet's physics transform
    void getWorldTransform(btTransform &centerOfMassWorldTrans) const override;

    // Called from bullet on transform update
    void setWorldTransform(const btTransform &centerOfMassWorldTrans) override;

    std::unique_ptr<btRigidBody> createRigidbody(bool isDynamic, btCollisionShape *shape);

    float _mass; // Setting mass to 0 makes this rigidbody static.
    glm::vec3 _size{1.f};

    std::unique_ptr<btRigidBody> _body{nullptr};
    std::unique_ptr<btCollisionShape> _optionalLocalShape{nullptr};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cRigidbody)

CEREAL_REGISTER_TYPE(cRigidbody)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cRigidbody)
