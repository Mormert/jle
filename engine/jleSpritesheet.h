// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFileLoadInterface.h"
#include "jleTexture.h"

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

    std::shared_ptr<jleTexture> _imageTexture;
    std::string _pathJson;
};

void from_json(const nlohmann::json& j, jleSpritesheet& s);

void from_json(const nlohmann::json& j, jleSpritesheetEntity& s);

void from_json(const nlohmann::json& j,
               jleSpritesheetEntity::jleSpritesheetEntityFrame& s);
