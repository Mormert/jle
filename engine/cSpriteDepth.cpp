// Copyright (c) 2023. Johan Lind

#include "cSpriteDepth.h"

#include "jleObject.h"

#include "jleGameEngine.h"
#include "jleResource.h"
#include "jleRendering.h"

cSpriteDepth::cSpriteDepth(jleObject *owner, jleScene *scene)
    : jleComponent{owner, scene} {}

void cSpriteDepth::createAndSetTextureFromPath(const std::string &pathDiffuse,
                                               const std::string &pathHeight,
                                               const std::string &pathNormal) {
    if (!quad.mtextureWithHeightmap) {
        quad.mtextureWithHeightmap = std::make_shared<TextureWithHeightmap>();
    }

    quad.mtextureWithHeightmap->texture =
        gCore->resources().loadResourceFromFile<jleTexture>(jleRelativePath{pathDiffuse});
    quad.mtextureWithHeightmap->heightmap =
        gCore->resources().loadResourceFromFile<jleTexture>(jleRelativePath{pathHeight});
    if (!pathNormal.empty()) {
        quad.mtextureWithHeightmap->normalmap =
            gCore->resources().loadResourceFromFile<jleTexture>(jleRelativePath{pathNormal});
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
        gCore->quadRendering().sendTexturedHeightQuad(*&quad);
    } else {
        gCore->quadRendering().sendSimpleTexturedHeightQuad(*&quad);
    }
}
