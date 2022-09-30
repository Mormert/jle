// Copyright (c) 2022. Johan Lind

#ifndef HEXABLO_OWORLD_H
#define HEXABLO_OWORLD_H

#include "hexHexagonItem.h"
#include "jleAseprite.h"
#include "jleObject.h"
#include "jleQuadRenderingInterface.h"
#include "jleTextureInterface.h"
#include <glm/glm.hpp>

#include <set>

class oWorld : public jleObject {
    JLE_REGISTER_OBJECT_TYPE(oWorld)
public:
    ~oWorld() override;

    oWorld();

    void SetupDefaultObject() override;

    void Start() override;

    void Update(float dt) override;

    void ToJson(nlohmann::json& j_out) override;

    void FromJson(const nlohmann::json& j_in) override;

    hexHexagonItem *GetHexItemAt(int q, int r);

    void RemoveHexItemAt(int q, int r);

    void PlaceHexItemAt(hexHexagonItem *item, int q, int r);

    bool IsHexagonWalkable(int q, int r);

    // access the one and only world globally
    inline static oWorld *sWorld;

    int mHexSizeX{}, mHexSizeY{};

private:
    void LoadTilesTexture();

    void GenerateVisualWorld();

    void RenderVisualWorld(float dt);

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
    // In debug mode we also use this mHexagonItems_debug to assure that
    // mHexagonItems for our purposes hashes correctly. Using double maps are
    // obviously slower since 2 lookups are required, and we only use it in
    // debug mode.
    std::unordered_map<int, std::unordered_map<int, hexHexagonItem *>>
        mHexagonItems_debug;
#endif

    std::unordered_map<std::pair<int, int>, hexHexagonItem *, pair_hash>
        mHexagonItems;

    std::array<std::array<int, 50>, 50> mWorldHexagons;

    std::set<std::pair<int, int>> mStaticallyNotWalkableHexagons;

    std::string mWorldHexTilesAsepritePath;
    jleAseprite mAseprite;

    jleQuadRenderingInterface *mQuadRenderingPtr{nullptr};

    struct HexagonTile {
        float mDepth;
        int mTextureX, mTextureY, mWidth, mHeight;
    };

    friend void from_json(const nlohmann::json& j, oWorld::HexagonTile& w);

    friend void to_json(nlohmann::json& j, const oWorld::HexagonTile& w);

    std::vector<HexagonTile> mHexagonTiles{5};
};

void from_json(const nlohmann::json& j, oWorld::HexagonTile& w);

void to_json(nlohmann::json& j, const oWorld::HexagonTile& w);

#endif // HEXABLO_OWORLD_H
