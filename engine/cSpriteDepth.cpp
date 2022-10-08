// Copyright (c) 2022. Johan Lind

#include "cSpriteDepth.h"

#include "jleObject.h"

#include "jleGameEngine.h"

cSpriteDepth::cSpriteDepth(jleObject *owner, jleScene *scene)
    : jleComponent{owner, scene} {}

void cSpriteDepth::createAndSetTextureFromPath(const std::string &pathDiffuse,
                                               const std::string &pathHeight,
                                               const std::string &pathNormal) {
    if (!quad.mtextureWithHeightmap) {
        quad.mtextureWithHeightmap = std::make_shared<TextureWithHeightmap>();
    }

    quad.mtextureWithHeightmap->texture =
        jleTexture::fromPath(jleRelativePath{pathDiffuse});
    quad.mtextureWithHeightmap->heightmap =
        jleTexture::fromPath(jleRelativePath{pathHeight});
    quad.mtextureWithHeightmap->normalmap =
        jleTexture::fromPath(jleRelativePath{pathNormal});
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
    transform = _attachedToObject->addDependencyComponent<cTransform>(this);

    if (texturePathHeight != "" && texturePathDiffuse != "" &&
        texturePathNormal != "") {
        createAndSetTextureFromPath(
            texturePathDiffuse, texturePathHeight, texturePathNormal);
    }
}

void cSpriteDepth::update(float dt) {
    quad.x = transform->worldX();
    quad.y = transform->worldY();

    if (!quad.mtextureWithHeightmap) {
        return;
    }

    if (quad.mtextureWithHeightmap->texture) {
        gCore->quadRendering().sendTexturedHeightQuad(*&quad);
    }
}

void cSpriteDepth::toJson(nlohmann::json &j_out) {
    j_out = nlohmann::json{{"pathDiffuse", texturePathDiffuse},
                           {"pathHeight", texturePathHeight},
                           {"pathNormal", texturePathNormal},
                           {"x", quad.x},
                           {"y", quad.y},
                           {"depth", quad.depth},
                           {"height", quad.height},
                           {"width", quad.width},
                           {"textureX", quad.textureX},
                           {"textureY", quad.textureY}};
}

void cSpriteDepth::fromJson(const nlohmann::json &j_in) {
    texturePathDiffuse = j_in.at("pathDiffuse");
    texturePathHeight = j_in.at("pathHeight");
    texturePathNormal = j_in.at("pathNormal");
    quad.x = j_in.at("x");
    quad.y = j_in.at("y");
    quad.depth = j_in.at("depth");
    quad.height = j_in.at("height");
    quad.width = j_in.at("width");
    quad.textureX = j_in.at("textureX");
    quad.textureY = j_in.at("textureY");

    createAndSetTextureFromPath(
        texturePathDiffuse, texturePathHeight, texturePathNormal);
}
