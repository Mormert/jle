// Copyright (c) 2023. Johan Lind

#include "jleSpritesheet.h"
#include "jleGameEngine.h"
#include "jleResource.h"
#include <filesystem>
#include <fstream>

void
jleSpritesheet::loadImage()
{
    std::string pngPath = _pathJson.substr(0, _pathJson.find(".", 0)) + ".png";

    _imageTexture = gCore->resources().loadResourceFromFile<jleTexture>(jlePath{pngPath});
}

jleLoadFromFileSuccessCode
jleSpritesheet::loadFromFile(const jlePath &path)
{
    _pathJson = path.getRealPath();
    std::ifstream i(path.getRealPath());
    if (i.good()) {
        nlohmann::json j;
        i >> j;

        from_json(j["frames"], *this);
        loadImage();
        return jleLoadFromFileSuccessCode::SUCCESS;
    } else {
        LOG_ERROR << "Could not load Spritesheet json file " << path.getRealPath();
        return jleLoadFromFileSuccessCode::FAIL;
    }
}

jleSpritesheet::jleSpritesheet(const jlePath &path)
{
    _pathJson = path.getRealPath();
    loadFromFile(path);
}
