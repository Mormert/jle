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

#include "cLight.h"

class cLightDirectional : public cLight
{
    JLE_REGISTER_COMPONENT_TYPE(cLightDirectional)
public:

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(cereal::base_class<cLight>(this));
    }

    void registerLua(sol::state& lua, sol::table &table) override;

    void update(float dt) override;

    void editorGizmosRender(bool selected) override;
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cLightDirectional)


CEREAL_REGISTER_TYPE(cLightDirectional)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cLight, cLightDirectional)
