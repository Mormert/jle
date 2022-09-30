// Copyright (c) 2022. Johan Lind

#ifndef JLESPRITESHEET_H
#define JLESPRITESHEET_H

#include "jleFileLoadInterface.h"
#include "jleTexture.h"

#include "json.hpp"

#include <string>
#include <unordered_map>

struct jleSpritesheetEntity {
    struct jleSpritesheetEntityFrame {
        int mX, mY, mWidth, mHeight;
    } mFrame;
};

struct jleSpritesheet : jleFileLoadInterface {

    jleSpritesheet() = default;

    explicit jleSpritesheet(const std::string& path);

    void LoadImage();

    bool LoadFromFile(const std::string& path) override;

    std::unordered_map<std::string, jleSpritesheetEntity> mSpritesheetEntities;

    std::shared_ptr<jleTexture> mImageTexture;
    std::string mPathJson;
};

void from_json(const nlohmann::json& j, jleSpritesheet& s);

void from_json(const nlohmann::json& j, jleSpritesheetEntity& s);

void from_json(const nlohmann::json& j,
               jleSpritesheetEntity::jleSpritesheetEntityFrame& s);

#endif // JLESPRITESHEET_H
