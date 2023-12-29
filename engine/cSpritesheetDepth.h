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

#include "cSpritesheet.h"
#include "jleQuads.h"

class cSpritesheetDepth : public cSpritesheet
{
    JLE_REGISTER_COMPONENT_TYPE(cSpritesheetDepth)
public:
    explicit cSpritesheetDepth(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_spritesheetPathDepth), CEREAL_NVP(_spritesheetPathDiffuse), CEREAL_NVP(_spritesheetPathNormal));
    }

    void createAndSetTextureFromPath(const std::string &pathDiffuse,
                                     const std::string &pathHeight,
                                     const std::string &pathNormal);

    void update(float dt) override;

protected:
    std::string _spritesheetPathDiffuse;
    std::string _spritesheetPathDepth;
    std::string _spritesheetPathNormal;
    jleTexturedHeightQuad _quad;
};

CEREAL_REGISTER_TYPE(cSpritesheetDepth)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cSpritesheetDepth)
