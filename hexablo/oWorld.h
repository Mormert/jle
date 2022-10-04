// Copyright (c) 2022. Johan Lind

#pragma once

#include "hexHexagonItem.h"
#include "jleAseprite.h"
#include "jleObject.h"
#include "jleQuadRendering.h"
#include "jleTexture.h"
#include <glm/glm.hpp>

#include <set>

class oWorld : public jleObject {
    JLE_REGISTER_OBJECT_TYPE(oWorld)
public:
    ~oWorld() override;

    oWorld();

    void upDefaultObject() override;

    void start() override;

    void update(float dt) override;

    void toJson(nlohmann::json& j_out) override;

    void fromJson(const nlohmann::json& j_in) override;

    hexHexagonItem *hexItemAt(int q, int r);

    void removeHexItemAt(int q, int r);

    void placeHexItemAt(hexHexagonItem *item, int q, int r);

    bool isHexagonWalkable(int q, int r);

    // access the one and only world globally
    inline static oWorld *sWorld;

    int _hexSizeX{}, _hexSizeY{};

private:
    void loadTilesTexture();

    void generateVisualWorld();

    void renderVisualWorld(float dt);

    struct pair_hash {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2>& p) const {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);

            // Mainly for demonstration purposes, i.e. works but is overly
            // simple In the real world, use sth. like boost.hash_combine
            return h1 ^ h2;
        }
    };

#ifndef NDEBUG
    // In debug mode we also use this _hexagonItems_debug to assure that
    // _hexagonItems for our purposes hashes correctly. Using double maps are
    // obviously slower since 2 lookups are required, and we only use it in
    // debug mode.
    std::unordered_map<int, std::unordered_map<int, hexHexagonItem *>>
        _hexagonItems_debug;
#endif

    std::unordered_map<std::pair<int, int>, hexHexagonItem *, pair_hash>
        _hexagonItems;

    std::array<std::array<int, 50>, 50> _worldHexagons;

    std::set<std::pair<int, int>> _staticallyNotWalkableHexagons;

    std::string _worldHexTilesAsepritePath;
    jleAseprite _aseprite;

    jleQuadRendering *_quadRenderingPtr{nullptr};

    struct HexagonTile {
        float _depth;
        int _textureX, _textureY, _width, _height;
    };

    friend void from_json(const nlohmann::json& j, oWorld::HexagonTile& w);

    friend void to_json(nlohmann::json& j, const oWorld::HexagonTile& w);

    std::vector<HexagonTile> _hexagonTiles{5};
};

void from_json(const nlohmann::json& j, oWorld::HexagonTile& w);

void to_json(nlohmann::json& j, const oWorld::HexagonTile& w);
