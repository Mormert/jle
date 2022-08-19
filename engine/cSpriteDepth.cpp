// Copyright (c) 2022. Johan Lind

#include "cSpriteDepth.h"

#include "jleObject.h"


#include "jleGameEngine.h"

cSpriteDepth::cSpriteDepth(jle::jleObject *owner, jle::jleScene *scene) : jleComponent{owner, scene} {
}

void cSpriteDepth::CreateAndSetTextureFromPath(const std::string &pathDiffuse, const std::string &pathHeight,
                                               const std::string &pathNormal) {
    if (!quad.mtextureWithHeightmap) {
        quad.mtextureWithHeightmap = std::make_shared<TextureWithHeightmap>();
    }

    quad.mtextureWithHeightmap->texture = jle::jleCore::core->texture_creator->LoadTextureFromPath(jleRelativePath{pathDiffuse});
    quad.mtextureWithHeightmap->heightmap = jle::jleCore::core->texture_creator->LoadTextureFromPath(jleRelativePath{pathHeight});
    quad.mtextureWithHeightmap->normalmap = jle::jleCore::core->texture_creator->LoadTextureFromPath(jleRelativePath{pathNormal});
}

void cSpriteDepth::SetRectangleDimensions(int width, int height) {
    quad.width = width;
    quad.height = height;
}

void cSpriteDepth::SetTextureBeginCoordinates(int x, int y) {
    quad.textureX = x;
    quad.textureY = y;
}

void cSpriteDepth::Start() {
    transform = mAttachedToObject->AddDependencyComponent<cTransform>(this);

    if (texturePathHeight != "" && texturePathDiffuse != "" && texturePathNormal != "") {
        CreateAndSetTextureFromPath(texturePathDiffuse, texturePathHeight, texturePathNormal);
    }

}

void cSpriteDepth::Update(float dt) {
    quad.x = transform->GetWorldX();
    quad.y = transform->GetWorldY();

    if (!quad.mtextureWithHeightmap) {
        return;
    }

    if (quad.mtextureWithHeightmap->texture) {
        jle::jleCore::core->rendering->quads->SendTexturedHeightQuad(*&quad, RenderingMethod::Dynamic);
    }
}

void cSpriteDepth::ToJson(nlohmann::json &j_out) {
    j_out = nlohmann::json{
            {"pathDiffuse", texturePathDiffuse},
            {"pathHeight",  texturePathHeight},
            {"pathNormal",  texturePathNormal},
            {"x",           quad.x},
            {"y",           quad.y},
            {"depth",       quad.depth},
            {"height",      quad.height},
            {"width",       quad.width},
            {"textureX",    quad.textureX},
            {"textureY",    quad.textureY}
    };
}

void cSpriteDepth::FromJson(const nlohmann::json &j_in) {
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

    CreateAndSetTextureFromPath(texturePathDiffuse, texturePathHeight, texturePathNormal);
}