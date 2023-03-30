// Copyright (c) 2023. Johan Lind

#include "jleSpritesheet.h"
#include "jleGameEngine.h"
#include "jleResource.h"
#include <filesystem>
#include <fstream>

void jleSpritesheet::loadImage() {
    std::string pngPath = _pathJson.substr(0, _pathJson.find(".", 0)) + ".png";

    _imageTexture = gCore->resources().loadResourceFromFile<jleTexture>(jleRelativePath{pngPath});
}

bool jleSpritesheet::loadFromFile(const std::string &path) {
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

jleSpritesheet::jleSpritesheet(const std::string &path) {
    _pathJson = path;
    loadFromFile(path);
}
