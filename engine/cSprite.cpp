// Copyright (c) 2022. Johan Lind

#include "cSprite.h"

#include "jleObject.h"

#include "jleGameEngine.h"

cSprite::cSprite(jleObject *owner, jleScene *scene)
    : jleComponent{owner, scene}, quad{nullptr} {}

void cSprite::CreateAndSetTextureFromPath(const std::string& path) {
    quad.texture = jleCore::core->texture_creator->LoadTextureFromPath(
        jleRelativePath{path});
}

void cSprite::SetTexture(std::shared_ptr<jleTexture> texture) {
    quad.texture = texture;
}

void cSprite::SetRectangleDimensions(int width, int height) {
    quad.width = width;
    quad.height = height;
}

void cSprite::SetTextureBeginCoordinates(int x, int y) {
    quad.textureX = x;
    quad.textureY = y;
}

void cSprite::Start() {
    transform = _attachedToObject->AddDependencyComponent<cTransform>(this);

    if (texturePath != "") {
        CreateAndSetTextureFromPath(texturePath);
    }
}

void cSprite::Update(float dt) {
    quad.x = transform->GetWorldX();
    quad.y = transform->GetWorldY();

    if (quad.texture.get()) {
        jleCore::core->rendering->quads().SendTexturedQuad(
            *&quad, RenderingMethod::Dynamic);
    }
}

void cSprite::ToJson(nlohmann::json& j_out) {
    j_out = nlohmann::json{{"path", texturePath},
                           {"x", quad.x},
                           {"y", quad.y},
                           {"depth", quad.depth},
                           {"height", quad.height},
                           {"width", quad.width},
                           {"textureX", quad.textureX},
                           {"textureY", quad.textureY}};
}

void cSprite::FromJson(const nlohmann::json& j_in) {
    texturePath = j_in.at("path");
    quad.x = j_in.at("x");
    quad.y = j_in.at("y");
    quad.depth = j_in.at("depth");
    quad.height = j_in.at("height");
    quad.width = j_in.at("width");
    quad.textureX = j_in.at("textureX");
    quad.textureY = j_in.at("textureY");

    CreateAndSetTextureFromPath(texturePath);
}
