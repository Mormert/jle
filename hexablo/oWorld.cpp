// Copyright (c) 2022. Johan Lind

#include "oWorld.h"
#include "cSprite.h"
#include "cTransform.h"
#include "hexHexagonFunctions.h"
#include "jleCore.h"
//#include "jleRenderingAPI_OpenGL.h"

#include "3rdparty/FastNoiseLite/FastNoiseLite.h"

void oWorld::SetupDefaultObject() {}

void oWorld::Start() {
    if (_aseprite._frames.empty()) {
        LoadTilesTexture();
    }
    _quadRenderingPtr = &jleCore::core->rendering->quads();

    GenerateVisualWorld();
}

void oWorld::Update(float dt) {

    //  RenderVisualWorld(dt);

    /*for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 25; j++) {
            const auto realCoords = hexHexagonFunctions::HexToPixel(i, j,
    _hexSizeX, _hexSizeY); quad.x = realCoords.x - hexagonTile._width / 2;
            quad.y = realCoords.y - hexagonTile._height / 2;
            quad.depth = hexagonTile._depth;
            _quadRenderingPtr->SendTexturedQuad(quad, RenderingMethod::Dynamic);
        }
    }

    const auto mx = hexHelperFunctions::GetPixelatedMouseXWorldSpace();
    const auto my = hexHelperFunctions::GetPixelatedMouseYWorldSpace();

    const auto hexCoord = hexHexagonFunctions::PixelToHex(mx, my, _hexSizeX,
    _hexSizeY);

    quad.textureX = _hexagonTiles[0]._textureX;
    quad.depth = 0.5f;
    const auto realCoords = hexHexagonFunctions::HexToPixel(hexCoord.x,
    hexCoord.y, _hexSizeX, _hexSizeY); quad.x = realCoords.x -
    hexagonTile._width / 2; quad.y = realCoords.y - hexagonTile._height / 2;
    _quadRenderingPtr->SendTexturedQuad(quad, RenderingMethod::Dynamic);*/
}

void oWorld::ToJson(nlohmann::json& j_out) {
    j_out["_worldHexTilesAsepritePath"] = _worldHexTilesAsepritePath;
    j_out["_hexagonTiles"] = _hexagonTiles;
    j_out["_hexSizeX"] = _hexSizeX;
    j_out["_hexSizeY"] = _hexSizeY;
}

void oWorld::FromJson(const nlohmann::json& j_in) {
    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _worldHexTilesAsepritePath, "_worldHexTilesAsepritePath", "");
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _hexSizeX, "_hexSizeX", 8);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _hexSizeY, "_hexSizeY", 6);
    const auto tiles = j_in.find("_hexagonTiles");
    if (tiles != j_in.end()) {
        _hexagonTiles =
            j_in.at("_hexagonTiles").get<std::vector<HexagonTile>>();
    }
    if (_aseprite._frames.empty()) {
        LoadTilesTexture();
    }
}

void oWorld::LoadTilesTexture() {
    if (!_worldHexTilesAsepritePath.empty()) {
        _aseprite.LoadFromFile(
            jleRelativePath{_worldHexTilesAsepritePath}.GetAbsolutePathStr());
        //_worldHexTilesTexture =
        // jleCore::core->texture_creator->LoadTextureFromPath(truePath);
    }
}

void from_json(const nlohmann::json& j, oWorld::HexagonTile& w) {
    w._depth = j.at("_depth");
    w._textureX = j.at("_textureX");
    w._textureY = j.at("_textureY");
    w._width = j.at("_width");
    w._height = j.at("_height");
}

void to_json(nlohmann::json& j, const oWorld::HexagonTile& w) {
    j = nlohmann::json{{"_depth", w._depth},
                       {"_textureX", w._textureX},
                       {"_textureY", w._textureY},
                       {"_width", w._width},
                       {"_height", w._height}};
}

oWorld::oWorld() { sWorld = this; }

oWorld::~oWorld() { sWorld = nullptr; }

hexHexagonItem *oWorld::GetHexItemAt(int q, int r) {

#ifndef NDEBUG

    auto item1 = _hexagonItems[{q, r}];
    auto item2 = _hexagonItems_debug[q][r];

    if (item1 != item2) {
        LOGE << "Hash functions did not find the same element. Consider using "
                "better hash.";
        std::exit(EXIT_FAILURE);
    }

    return item1;

#else
    return _hexagonItems[{q, r}];
#endif
}

void oWorld::RemoveHexItemAt(int q, int r) {
    _hexagonItems.erase({q, r});

#ifndef NDEBUG
    _hexagonItems_debug[q].erase(r);
#endif
}

void oWorld::PlaceHexItemAt(hexHexagonItem *item, int q, int r) {
    _hexagonItems[{q, r}] = item;

#ifndef NDEBUG
    _hexagonItems_debug[q][r] = item;
#endif
}

bool oWorld::IsHexagonWalkable(int q, int r) {
    // Check for static hexagons, like water, rocks, etc first, since they are
    // the most common
    if (_staticallyNotWalkableHexagons.find({q, r}) !=
        _staticallyNotWalkableHexagons.end()) {
        return false;
    }

    if (_hexagonItems[{q, r}]) {
        return false;
    }

    return true;
}

void oWorld::GenerateVisualWorld() {

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    int index = 0;
    for (int i = 0; i < _worldHexagons.size(); i++) {
        for (int j = 0; j < _worldHexagons[i].size(); j++) {
            const int hexagonID =
                (noise.GetNoise(((float)i) * 10.f, ((float)j) * 10.f) + 1.f) *
                2.f;
            _worldHexagons[i][j] = hexagonID;

            if (hexagonID == 3) {
                _staticallyNotWalkableHexagons.insert({i, j});
            }
        }
    }
}

void oWorld::RenderVisualWorld(float dt) {

    static int currentFrame = _aseprite._frames.size();
    static float currentFrameTimeSpent = 0;
    static float currentFrameDurationMs = 0;

    currentFrameTimeSpent += dt * 1000.f;
    if (currentFrameTimeSpent >= currentFrameDurationMs) {
        currentFrame++;
        if (currentFrame >= _aseprite._frames.size()) {
            currentFrame = 0;
        }
        currentFrameTimeSpent = 0;
        currentFrameDurationMs = _aseprite._frames.at(currentFrame)._duration;
    }

    for (int i = 0; i < _worldHexagons.size(); i++) {
        for (int j = 0; j < _worldHexagons[i].size(); j++) {
            const auto& hexagonTile = _hexagonTiles[_worldHexagons[i][j]];

            const auto& frame = _aseprite._frames.at(currentFrame);

            TexturedQuad quad{_aseprite._imageTexture,
                              frame._frame._x + hexagonTile._textureX,
                              frame._frame._y + hexagonTile._textureY,
                              static_cast<unsigned int>(hexagonTile._width),
                              static_cast<unsigned int>(hexagonTile._height)};

            const auto realCoords =
                hexHexagonFunctions::HexToPixel(i, j, _hexSizeX, _hexSizeY);
            quad.x = realCoords.x - hexagonTile._width / 2;
            quad.y = realCoords.y - hexagonTile._height / 2;
            quad.depth = hexagonTile._depth;
            _quadRenderingPtr->SendTexturedQuad(quad, RenderingMethod::Dynamic);
        }
    }
}
