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

#include "jleComponent.h"

#include <glm/vec3.hpp>

class cLight : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cLight)
public:

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_color));
    }

    void start() override;

    void update(float dt) override;

    void editorUpdate(float dt) override;

    void editorGizmosRender(bool selected) override;

    void registerLua(sol::state& lua) override;

protected:
    glm::vec3 _color{1.f};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cLight)


CEREAL_REGISTER_TYPE(cLight)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cLight)
