// Copyright (c) 2022. Johan Lind

#include "jleSpritesheet.h"
#include "jleGameEngine.h"

#include <filesystem>
#include <fstream>

void from_json(const nlohmann::json& j, jleSpritesheet& s) {
    s._spritesheetEntities =
        j.get<std::unordered_map<std::string, jleSpritesheetEntity>>();
}

void from_json(const nlohmann::json& j, jleSpritesheetEntity& s) {
    s._frame = j["frame"];
}

void from_json(const nlohmann::json& j,
               jleSpritesheetEntity::jleSpritesheetEntityFrame& s) {
    s._x = j["x"];
    s._y = j["y"];
    s._width = j["width"];
    s._height = j["height"];
}

void jleSpritesheet::LoadImage() {
    std::string pngPath = _pathJson.substr(0, _pathJson.find(".", 0)) + ".png";

    _imageTexture = jleCore::core->texture_creator->LoadTextureFromPath(
        jleRelativePath{pngPath});
}

bool jleSpritesheet::LoadFromFile(const std::string& path) {
    _pathJson = path;
    std::ifstream i(path);
    if (i.good()) {
        nlohmann::json j;
        i >> j;

        from_json(j, *this);
        LoadImage();
        return true;
    }
    else {
        LOG_ERROR << "Could not load Spritesheet json file " << path;
        return false;
    }
}

jleSpritesheet::jleSpritesheet(const std::string& path) {
    _pathJson = path;
    LoadFromFile(path);
}
