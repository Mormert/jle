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
#include "jleTransform.h"

#include "jleQuads.h"
#include "jleTexture.h"

#include <memory>
#include <string>

class cSpriteDepth : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cSpriteDepth)
public:
    explicit cSpriteDepth(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(texturePathDiffuse),
           CEREAL_NVP(texturePathHeight),
           CEREAL_NVP(texturePathNormal),
           CEREAL_NVP(quad.x),
           CEREAL_NVP(quad.y),
           CEREAL_NVP(quad.depth),
           CEREAL_NVP(quad.height),
           CEREAL_NVP(quad.width),
           CEREAL_NVP(quad.textureX),
           CEREAL_NVP(quad.textureY));
    }

    void createAndSetTextureFromPath(const std::string &pathDiffuse,
                                     const std::string &pathHeight,
                                     const std::string &pathNormal);

    void rectangleDimensions(int width, int height);

    void textureBeginCoordinates(int x, int y);

    virtual void start() override;

    virtual void update(float dt) override;

private:
    std::string texturePathDiffuse = "";
    std::string texturePathHeight = "";
    std::string texturePathNormal = "";

    jleTexturedHeightQuad quad;
};

CEREAL_REGISTER_TYPE(cSpriteDepth)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cSpriteDepth)
