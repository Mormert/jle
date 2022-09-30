// Copyright (c) 2022. Johan Lind

#ifndef JLESPRITESHEET_H
#define JLESPRITESHEET_H

#include "jleFileLoadInterface.h"
#include "jleTextureInterface.h"

#include "json.hpp"

#include <string>
#include <unordered_map>

struct jleSpritesheetEntity {
    struct jleSpritesheetEntityFrame {
        int _x, _y, _width, _height;
    } _frame;
};

struct jleSpritesheet : jleFileLoadInterface {

    jleSpritesheet() = default;

    explicit jleSpritesheet(const std::string& path);

    void LoadImage();

    bool LoadFromFile(const std::string& path) override;

    std::unordered_map<std::string, jleSpritesheetEntity> _spritesheetEntities;

    std::shared_ptr<jleTextureInterface> _imageTexture;
    std::string _pathJson;
};

void from_json(const nlohmann::json& j, jleSpritesheet& s);

void from_json(const nlohmann::json& j, jleSpritesheetEntity& s);

void from_json(const nlohmann::json& j,
               jleSpritesheetEntity::jleSpritesheetEntityFrame& s);

#endif // JLESPRITESHEET_H
