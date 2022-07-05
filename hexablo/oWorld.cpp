// Copyright (c) 2022. Johan Lind

#include "oWorld.h"
#include "jleCore.h"
#include "cTransform.h"
#include "cSprite.h"
#include "hexHexagonFunctions.h"
#include "hexHelperFunctions.h"

void oWorld::SetupDefaultObject() {
    /* for (int i = -20; i < 20; i++) {
         for (int j = -30; j < 30; j++) {
             auto child = SpawnChildObject<jle::jleObject>();
             auto t = child->AddCustomComponent<cTransform>();
             const auto realCoords = HexToPixel(i, j, 8, 6);
             t->SetLocalPosition((int) realCoords.x, (int) realCoords.y);

             auto s = child->AddCustomComponent<cSprite>();
             s->SetTextureBeginCoordinates(1, 1);
             s->SetRectangleDimensions(15, 13);
             s->CreateAndSetTextureFromPath("GR:worldHexTiles.png");

             // mQuadRenderingPtr->SendTexturedQuad(quad, RenderingMethod::Dynamic);
         }
     }*/

}

void oWorld::Start() {
    if (!mWorldHexTilesTexture) {
        LoadTilesTexture();
    }
    mQuadRenderingPtr = jle::jleCore::core->rendering->quads.get();
}

void oWorld::Update(float dt) {
    if (!mWorldHexTilesTexture) {
        return;
    }

    const auto &hexagonTile = mHexagonTiles[1];

    TexturedQuad quad{mWorldHexTilesTexture, hexagonTile.mTextureX, hexagonTile.mTextureY,
                      static_cast<unsigned int>(hexagonTile.mWidth), static_cast<unsigned int>(hexagonTile.mHeight)};

    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 25; j++) {
            const auto realCoords = hexHexagonFunctions::HexToPixel(i, j, mHexSizeX, mHexSizeY);
            quad.x = realCoords.x - hexagonTile.mWidth / 2;
            quad.y = realCoords.y - hexagonTile.mHeight / 2;
            quad.depth = hexagonTile.mDepth;
            mQuadRenderingPtr->SendTexturedQuad(quad, RenderingMethod::Dynamic);
        }
    }

    const auto mx = hexHelperFunctions::GetPixelatedMouseXWorldSpace();
    const auto my = hexHelperFunctions::GetPixelatedMouseYWorldSpace();

    const auto hexCoord = hexHexagonFunctions::PixelToHex(mx, my, mHexSizeX, mHexSizeY);

    quad.textureX = mHexagonTiles[0].mTextureX;
    quad.depth = 0.5f;
    const auto realCoords = hexHexagonFunctions::HexToPixel(hexCoord.x, hexCoord.y, mHexSizeX, mHexSizeY);
    quad.x = realCoords.x - hexagonTile.mWidth / 2;
    quad.y = realCoords.y - hexagonTile.mHeight / 2;
    mQuadRenderingPtr->SendTexturedQuad(quad, RenderingMethod::Dynamic);

}

void oWorld::ToJson(nlohmann::json &j_out) {
    j_out["mWorldHexTilesPath"] = mWorldHexTilesPath;
    j_out["mHexagonTiles"] = mHexagonTiles;
    j_out["mHexSizeX"] = mHexSizeX;
    j_out["mHexSizeY"] = mHexSizeY;
}


void oWorld::FromJson(const nlohmann::json &j_in) {
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mWorldHexTilesPath, "mWorldHexTilesPath", "");
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mHexSizeX, "mHexSizeX", 8);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mHexSizeY, "mHexSizeY", 6);
    mHexSizeX_inverse = 1.f / (float) mHexSizeX;
    mHexSizeY_inverse = 1.f / (float) mHexSizeY;
    const auto tiles = j_in.find("mHexagonTiles");
    if (tiles != j_in.end()) {
        mHexagonTiles = j_in.at("mHexagonTiles").get<std::vector<HexagonTile>>();
    }
    if (!mWorldHexTilesTexture) {
        LoadTilesTexture();
    }
}

void oWorld::LoadTilesTexture() {
    const auto &truePath = jle::FindTrueResourcePath(mWorldHexTilesPath);
    if (!truePath.empty()) {
        mWorldHexTilesTexture = jle::jleCore::core->texture_creator->LoadTextureFromPath(truePath);
    }
}

void from_json(const nlohmann::json &j, oWorld::HexagonTile &w) {
    w.mDepth = j.at("mDepth");
    w.mTextureX = j.at("mTextureX");
    w.mTextureY = j.at("mTextureY");
    w.mWidth = j.at("mWidth");
    w.mHeight = j.at("mHeight");
}

void to_json(nlohmann::json &j, const oWorld::HexagonTile &w) {
    j = nlohmann::json{
            {"mDepth",    w.mDepth},
            {"mTextureX", w.mTextureX},
            {"mTextureY", w.mTextureY},
            {"mWidth",    w.mWidth},
            {"mHeight",   w.mHeight}
    };
}