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

#include "cSprite.h"

#include "jleObject.h"

#include "jleGameEngine.h"

#include "jleResource.h"

cSprite::cSprite(jleObject *owner, jleScene *scene)
    : jleComponent{owner, scene}, quad{nullptr} {}

void cSprite::createAndSetTextureFromPath(const std::string &path) {
    quad.texture = gEngine->resources().loadResourceFromFile<jleTexture>(jlePath{path});
}

void cSprite::texture(std::shared_ptr<jleTexture> texture) {
    quad.texture = texture;
}

void cSprite::rectangleDimensions(int width, int height) {
    quad.width = width;
    quad.height = height;
}

void cSprite::textureBeginCoordinates(int x, int y) {
    quad.textureX = x;
    quad.textureY = y;
}

void cSprite::start() {
    if (texturePath != "") {
        createAndSetTextureFromPath(texturePath);
    }
}

void cSprite::update(float dt)
{
    quad.x = getTransform().getWorldPosition().x;
    quad.y = getTransform().getWorldPosition().y;

    if (quad.texture.get()) {
        gEngine->quadRendering().sendTexturedQuad(*&quad);
    }
}
