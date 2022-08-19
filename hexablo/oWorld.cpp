// Copyright (c) 2022. Johan Lind

#include "oWorld.h"
#include "jleCore.h"
#include "cTransform.h"
#include "cSprite.h"
#include "hexHexagonFunctions.h"

#include "3rdparty/FastNoiseLite/FastNoiseLite.h"

void oWorld::SetupDefaultObject() {
}

void oWorld::Start() {
    if (mAseprite.mFrames.empty()) {
        LoadTilesTexture();
    }
    mQuadRenderingPtr = jle::jleCore::core->rendering->quads.get();

    GenerateVisualWorld();
}

void oWorld::Update(float dt) {

  //  RenderVisualWorld(dt);

    /*for (int i = 0; i < 25; i++) {
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
    mQuadRenderingPtr->SendTexturedQuad(quad, RenderingMethod::Dynamic);*/

}

void oWorld::ToJson(nlohmann::json &j_out) {
    j_out["mWorldHexTilesAsepritePath"] = mWorldHexTilesAsepritePath;
    j_out["mHexagonTiles"] = mHexagonTiles;
    j_out["mHexSizeX"] = mHexSizeX;
    j_out["mHexSizeY"] = mHexSizeY;
}


void oWorld::FromJson(const nlohmann::json &j_in) {
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mWorldHexTilesAsepritePath, "mWorldHexTilesAsepritePath", "");
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mHexSizeX, "mHexSizeX", 8);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mHexSizeY, "mHexSizeY", 6);
    const auto tiles = j_in.find("mHexagonTiles");
    if (tiles != j_in.end()) {
        mHexagonTiles = j_in.at("mHexagonTiles").get<std::vector<HexagonTile>>();
    }
    if (mAseprite.mFrames.empty()) {
        LoadTilesTexture();
    }
}

void oWorld::LoadTilesTexture() {
    if (!mWorldHexTilesAsepritePath.empty()) {
        mAseprite.LoadFromFile(jleRelativePath{mWorldHexTilesAsepritePath}.GetAbsolutePathStr());
        //mWorldHexTilesTexture = jle::jleCore::core->texture_creator->LoadTextureFromPath(truePath);
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

oWorld::oWorld() {
    sWorld = this;
}

oWorld::~oWorld() {
    sWorld = nullptr;
}

hexHexagonItem *oWorld::GetHexItemAt(int q, int r) {

#ifndef NDEBUG

    auto item1 = mHexagonItems[{q, r}];
    auto item2 = mHexagonItems_debug[q][r];

    if (item1 != item2) {
        LOGE << "Hash functions did not find the same element. Consider using better hash.";
        std::exit(EXIT_FAILURE);
    }

    return item1;

#else
    return mHexagonItems[{q, r}];
#endif

}

void oWorld::RemoveHexItemAt(int q, int r) {
    mHexagonItems.erase({q, r});

#ifndef NDEBUG
    mHexagonItems_debug[q].erase(r);
#endif
}

void oWorld::PlaceHexItemAt(hexHexagonItem *item, int q, int r) {
    mHexagonItems[{q, r}] = item;

#ifndef NDEBUG
    mHexagonItems_debug[q][r] = item;
#endif
}

bool oWorld::IsHexagonWalkable(int q, int r) {
    // Check for static hexagons, like water, rocks, etc first, since they are the most common
    if (mStaticallyNotWalkableHexagons.find({q, r}) != mStaticallyNotWalkableHexagons.end()) {
        return false;
    }

    if (mHexagonItems[{q, r}]) {
        return false;
    }

    return true;
}

void oWorld::GenerateVisualWorld() {

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    int index = 0;
    for (int i = 0; i < mWorldHexagons.size(); i++) {
        for (int j = 0; j < mWorldHexagons[i].size(); j++) {
            const int hexagonID = (noise.GetNoise(((float) i) * 10.f, ((float) j) * 10.f) + 1.f) * 2.f;
            mWorldHexagons[i][j] = hexagonID;

            if (hexagonID == 3) {
                mStaticallyNotWalkableHexagons.insert({i, j});
            }

        }
    }
}

void oWorld::RenderVisualWorld(float dt) {

    static int currentFrame = mAseprite.mFrames.size();
    static float currentFrameTimeSpent = 0;
    static float currentFrameDurationMs = 0;

    currentFrameTimeSpent += dt * 1000.f;
    if (currentFrameTimeSpent >= currentFrameDurationMs) {
        currentFrame++;
        if (currentFrame >= mAseprite.mFrames.size()) {
            currentFrame = 0;
        }
        currentFrameTimeSpent = 0;
        currentFrameDurationMs = mAseprite.mFrames.at(currentFrame).mDuration;
    }

    for (int i = 0; i < mWorldHexagons.size(); i++) {
        for (int j = 0; j < mWorldHexagons[i].size(); j++) {
            const auto &hexagonTile = mHexagonTiles[mWorldHexagons[i][j]];

            const auto &frame = mAseprite.mFrames.at(currentFrame);

            TexturedQuad quad{mAseprite.mImageTexture, frame.mFrame.mX + hexagonTile.mTextureX, frame.mFrame.mY+ hexagonTile.mTextureY,
                              static_cast<unsigned int>(hexagonTile.mWidth),
                              static_cast<unsigned int>(hexagonTile.mHeight)};

            const auto realCoords = hexHexagonFunctions::HexToPixel(i, j, mHexSizeX, mHexSizeY);
            quad.x = realCoords.x - hexagonTile.mWidth / 2;
            quad.y = realCoords.y - hexagonTile.mHeight / 2;
            quad.depth = hexagonTile.mDepth;
            mQuadRenderingPtr->SendTexturedQuad(quad, RenderingMethod::Dynamic);
        }
    }
}
