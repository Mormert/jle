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

#include "cSpriteDepth.h"

#include "jleObject.h"

#include "jleGameEngine.h"
#include "jleResource.h"


cSpriteDepth::cSpriteDepth(jleObject *owner, jleScene *scene)
    : jleComponent{owner, scene} {}

void cSpriteDepth::createAndSetTextureFromPath(const std::string &pathDiffuse,
                                               const std::string &pathHeight,
                                               const std::string &pathNormal) {
    if (!quad.mtextureWithHeightmap) {
        quad.mtextureWithHeightmap = std::make_shared<TextureWithHeightmap>();
    }

    quad.mtextureWithHeightmap->texture =
        gEngine->resources().loadResourceFromFile<jleTexture>(jlePath{pathDiffuse});
    quad.mtextureWithHeightmap->heightmap =
        gEngine->resources().loadResourceFromFile<jleTexture>(jlePath{pathHeight});
    if (!pathNormal.empty()) {
        quad.mtextureWithHeightmap->normalmap =
            gEngine->resources().loadResourceFromFile<jleTexture>(jlePath{pathNormal});
    }
}

void cSpriteDepth::rectangleDimensions(int width, int height) {
    quad.width = width;
    quad.height = height;
}

void cSpriteDepth::textureBeginCoordinates(int x, int y) {
    quad.textureX = x;
    quad.textureY = y;
}

void cSpriteDepth::start() {
    if (texturePathHeight != "" && texturePathDiffuse != "") {
        createAndSetTextureFromPath(
            texturePathDiffuse, texturePathHeight, texturePathNormal);
    }
}

void cSpriteDepth::update(float dt)
{
    quad.x = getTransform().getWorldPosition().x;
    quad.y = getTransform().getWorldPosition().y;

    if (!quad.mtextureWithHeightmap) {
        return;
    }

    if (quad.mtextureWithHeightmap->normalmap) {
        gEngine->quadRendering().sendTexturedHeightQuad(*&quad);
    } else {
        gEngine->quadRendering().sendSimpleTexturedHeightQuad(*&quad);
    }
}
