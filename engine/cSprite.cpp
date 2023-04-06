// Copyright (c) 2023. Johan Lind

#include "cSprite.h"

#include "jleObject.h"

#include "jleGameEngine.h"
#include "jleRendering.h"
#include "jleResource.h"

cSprite::cSprite(jleObject *owner, jleScene *scene)
    : jleComponent{owner, scene}, quad{nullptr} {}

void cSprite::createAndSetTextureFromPath(const std::string &path) {
    quad.texture = gCore->resources().loadResourceFromFile<jleTexture>(jlePath{path});
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
        gCore->quadRendering().sendTexturedQuad(*&quad);
    }
}
