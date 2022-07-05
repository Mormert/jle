// Copyright (c) 2022. Johan Lind

#ifndef HEXABLO_OWORLD_H
#define HEXABLO_OWORLD_H

#include "jleObject.h"
#include "iTexture.h"
#include "iQuadRendering.h"
#include <glm/glm.hpp>

class oWorld : public jle::jleObject {
    JLE_REGISTER_OBJECT_TYPE(oWorld)
public:

    void SetupDefaultObject() override;

    void Start() override;

    void Update(float dt) override;

    void ToJson(nlohmann::json &j_out) override;

    void FromJson(const nlohmann::json &j_in) override;

private:
    void LoadTilesTexture();

    std::shared_ptr<jle::iTexture> mWorldHexTilesTexture{nullptr};
    std::string mWorldHexTilesPath;

    iQuadRendering* mQuadRenderingPtr{nullptr};

    struct HexagonTile {
        float mDepth;
        int mTextureX, mTextureY, mWidth, mHeight;
    };

    friend void from_json(const nlohmann::json &j, oWorld::HexagonTile &w);
    friend void to_json(nlohmann::json &j, const oWorld::HexagonTile &w);

    std::vector<HexagonTile> mHexagonTiles{5};

    int mHexSizeX, mHexSizeY;
    float mHexSizeX_inverse, mHexSizeY_inverse;
};

void from_json(const nlohmann::json &j, oWorld::HexagonTile &w);
void to_json(nlohmann::json &j, const oWorld::HexagonTile &w);



#endif //HEXABLO_OWORLD_H
