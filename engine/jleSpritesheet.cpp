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
    s.frame = j["frame"];
    // TODO: Should backward compatability be handled for this struct?
    s.sourceSize = j.value("spriteSourceSize",
                           jleSpritesheetEntity::jleSpritesheetEntitySource{});
}

void from_json(const nlohmann::json& j,
               jleSpritesheetEntity::jleSpritesheetEntityFrame& s) {
    s.x = j["x"];
    s.y = j["y"];
    s.width = j["w"];
    s.height = j["h"];
}

void from_json(const json& j,
               jleSpritesheetEntity::jleSpritesheetEntitySource& s) {
    s.x = j["x"];
    s.y = j["y"];
    s.width = j["w"];
    s.height = j["h"];
}

void jleSpritesheet::loadImage() {
    std::string pngPath = _pathJson.substr(0, _pathJson.find(".", 0)) + ".png";

    _imageTexture = jleTexture::fromPath(jleRelativePath{pngPath});
}

bool jleSpritesheet::loadFromFile(const std::string& path) {
    _pathJson = path;
    std::ifstream i(path);
    if (i.good()) {
        nlohmann::json j;
        i >> j;

        from_json(j["frames"], *this);
        loadImage();
        return true;
    }
    else {
        LOG_ERROR << "Could not load Spritesheet json file " << path;
        return false;
    }
}

jleSpritesheet::jleSpritesheet(const std::string& path) {
    _pathJson = path;
    loadFromFile(path);
}
