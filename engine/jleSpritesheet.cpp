// Copyright (c) 2022. Johan Lind

#include "jleSpritesheet.h"
#include "jleGameEngine.h"

#include <fstream>
#include <filesystem>

void from_json(const nlohmann::json &j, jleSpritesheet &s) {
   s.mSpritesheetEntities = j.get<std::unordered_map<std::string, jleSpritesheetEntity>>();
}

void from_json(const nlohmann::json &j, jleSpritesheetEntity &s) {
    s.mFrame = j["frame"];
}

void from_json(const nlohmann::json &j, jleSpritesheetEntity::jleSpritesheetEntityFrame &s) {
    s.mX = j["x"];
    s.mY = j["y"];
    s.mWidth = j["width"];
    s.mHeight = j["height"];
}

void jleSpritesheet::LoadImage() {
    std::string pngPath = mPathJson.substr(0, mPathJson.find(".", 0)) + ".png";

    mImageTexture = jle::jleCore::core->texture_creator->LoadTextureFromPath(jleRelativePath{pngPath});
}

bool jleSpritesheet::LoadFromFile(const std::string &path) {
    mPathJson = path;
    std::ifstream i(path);
    if (i.good()) {
        nlohmann::json j;
        i >> j;

        from_json(j, *this);
        LoadImage();
        return true;
    } else {
        LOG_ERROR << "Could not load Spritesheet json file " << path;
        return false;
    }
}

jleSpritesheet::jleSpritesheet(const std::string &path) {
    mPathJson = path;
    LoadFromFile(path);
}
