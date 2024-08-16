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

#include "jleBuildConfig.h"

#include "jleComponent.h"
#include "jleResourceRef.h"
#include "jleSkybox.h"
#include "modules/graphics/jle3DSettings.h"

class cSkybox : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cSkybox)
public:

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_skybox));

        if(_skybox.get())
        {
            gEngine->renderSettings().skybox = _skybox;
        }
    }

    void start() override;

    void update(float dt) override;

protected:
    jleResourceRef<jleSkybox> _skybox;
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cSkybox)

CEREAL_REGISTER_TYPE(cSkybox)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cSkybox)
