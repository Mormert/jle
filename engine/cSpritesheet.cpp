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

#include "cSpritesheet.h"
#include "jleGameEngine.h"
#include "jleGameEngine.h"
#include "jleQuadRendering.h"
#include "jleResource.h"
#include "json.hpp"
#include <plog/Log.h>

void cSpritesheet::start() {
}


void cSpritesheet::update(float dt) {
    if (!_spritesheet) {
        return;
    }

    auto &texture = _spritesheet->_imageTexture;
    if (texture != nullptr && _hasEntity) {
        texturedQuad quad{texture};
        quad.x = getTransform().getWorldPosition().x + _spritesheetEntityCache.sourceSize.x - _offset.x;
        quad.y = getTransform().getWorldPosition().y + _spritesheetEntityCache.sourceSize.y - _offset.y;

        quad.height = _spritesheetEntityCache.frame.height;
        quad.width = _spritesheetEntityCache.frame.width;
        quad.textureX = _spritesheetEntityCache.frame.x;
        quad.textureY = _spritesheetEntityCache.frame.y;
        quad.depth = getTransform().getWorldPosition().z;

        if (quad.texture.get()) {
            gEngine->quadRendering().sendTexturedQuad(quad);
        }
    }
}

void cSpritesheet::entity(const std::string &entityName) {
    if (!_spritesheet) {
        return;
    }

    _spriteName = entityName;
    const auto entity = _spritesheet->_spritesheetEntities.find(_spriteName);
    if (entity != _spritesheet->_spritesheetEntities.end()) {
        _spritesheetEntityCache = entity->second;
        _hasEntity = true;
    }
    else {
        LOGW << "Could not find sprite entity on the spritesheet: "
             << _spriteName;
    }
}
