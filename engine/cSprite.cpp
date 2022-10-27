// Copyright (c) 2022. Johan Lind

#include "cSprite.h"

#include "jleObject.h"

#include "jleGameEngine.h"

cSprite::cSprite(jleObject *owner, jleScene *scene)
    : jleComponent{owner, scene}, quad{nullptr} {}

void cSprite::createAndSetTextureFromPath(const std::string &path) {
    quad.texture = jleTexture::fromPath(jleRelativePath{path});
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
    transform = _attachedToObject->addDependencyComponent<cTransform>(this);

    if (texturePath != "") {
        createAndSetTextureFromPath(texturePath);
    }
}

void cSprite::update(float dt) {
    quad.x = transform->worldPosition().x;
    quad.y = transform->worldPosition().y;

    if (quad.texture.get()) {
        gCore->quadRendering().sendTexturedQuad(*&quad);
    }
}

void cSprite::toJson(nlohmann::json &j_out) {
    j_out = nlohmann::json{{"path", texturePath},
                           {"x", quad.x},
                           {"y", quad.y},
                           {"depth", quad.depth},
                           {"height", quad.height},
                           {"width", quad.width},
                           {"textureX", quad.textureX},
                           {"textureY", quad.textureY}};
}

void cSprite::fromJson(const nlohmann::json &j_in) {
    texturePath = j_in.at("path");
    quad.x = j_in.at("x");
    quad.y = j_in.at("y");
    quad.depth = j_in.at("depth");
    quad.height = j_in.at("height");
    quad.width = j_in.at("width");
    quad.textureX = j_in.at("textureX");
    quad.textureY = j_in.at("textureY");

    createAndSetTextureFromPath(texturePath);
}
