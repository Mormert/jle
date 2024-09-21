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

class cPhysicsShape : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cPhysicsShape)
public:
    cPhysicsShape() = default;

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        //ar(CEREAL_NVP(_mass));
    }

    void editorUpdate(jleEngineModulesContext& ctx) override;

    void start() override;

    void update(jleEngineModulesContext& ctx) override;

    void onDestroy(jleEngineModulesContext& ctx) override;

protected:
    //std::unique_ptr<btCollisionShape> _optionalLocalShape{nullptr};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cPhysicsShape)

CEREAL_REGISTER_TYPE(cPhysicsShape)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cPhysicsShape)

