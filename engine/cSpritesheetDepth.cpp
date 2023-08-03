// Copyright (c) 2023. Johan Lind

#include "cSpritesheetDepth.h"
#include "jleGameEngine.h"
#include "jleGameEngine.h"
#include "jleQuadRendering.h"
#include "jleResource.h"
#include "json.hpp"

cSpritesheetDepth::cSpritesheetDepth(jleObject *owner, jleScene *scene)
    : cSpritesheet(owner, scene) {}

void cSpritesheetDepth::update(float dt) {
    if (!_spritesheet || !_quad.mtextureWithHeightmap) {
        return;
    }

    auto &texture = _spritesheet->_imageTexture;
    if (texture != nullptr && _hasEntity) {
        _quad.x = getTransform().getWorldPosition().x + _spritesheetEntityCache.sourceSize.x - _offset.x;
        _quad.y = getTransform().getWorldPosition().y + _spritesheetEntityCache.sourceSize.y - _offset.y;

        _quad.height = _spritesheetEntityCache.frame.height;
        _quad.width = _spritesheetEntityCache.frame.width;
        _quad.textureX = _spritesheetEntityCache.frame.x;
        _quad.textureY = _spritesheetEntityCache.frame.y;
        _quad.depth = getTransform().getWorldPosition().z;

        if (_quad.mtextureWithHeightmap->normalmap) {
            gEngine->quadRendering().sendTexturedHeightQuad(*&_quad);
        }
        else if (_quad.mtextureWithHeightmap->heightmap) {
            gEngine->quadRendering().sendSimpleTexturedHeightQuad(*&_quad);
        }
    }
}

void cSpritesheetDepth::createAndSetTextureFromPath(
    const std::string &pathDiffuse,
    const std::string &pathHeight,
    const std::string &pathNormal) {
    if (!_quad.mtextureWithHeightmap) {
        _quad.mtextureWithHeightmap = std::make_shared<TextureWithHeightmap>();
    }

    _quad.mtextureWithHeightmap->texture =
        gEngine->resources().loadResourceFromFile<jleTexture>(jlePath{pathDiffuse});
    _quad.mtextureWithHeightmap->heightmap =
        gEngine->resources().loadResourceFromFile<jleTexture>(jlePath{pathHeight});
    if (!pathNormal.empty()) {
        _quad.mtextureWithHeightmap->normalmap =
            gEngine->resources().loadResourceFromFile<jleTexture>(jlePath{pathNormal});
    }
}
