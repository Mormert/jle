// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleFileLoadInterface.h"
#include "jleTexture.h"

#include "json.hpp"

#include <string>
#include <unordered_map>

struct jleSpritesheetEntity {
    struct jleSpritesheetEntityFrame {
        int x, y, width, height;
    } frame;

    struct jleSpritesheetEntitySource {
        int x, y, width, height;
    } sourceSize;
};

struct jleSpritesheet : public jleFileLoadInterface {

    jleSpritesheet() = default;

    explicit jleSpritesheet(const std::string &path);

    void loadImage();

    bool loadFromFile(const std::string &path) override;

    std::unordered_map<std::string, jleSpritesheetEntity> _spritesheetEntities;

    std::shared_ptr<jleTexture> _imageTexture;
    std::string _pathJson;
};

void from_json(const nlohmann::json &j, jleSpritesheet &s);

void from_json(const nlohmann::json &j, jleSpritesheetEntity &s);

void from_json(const nlohmann::json &j,
               jleSpritesheetEntity::jleSpritesheetEntityFrame &s);

void from_json(const nlohmann::json &j,
               jleSpritesheetEntity::jleSpritesheetEntitySource &s);
