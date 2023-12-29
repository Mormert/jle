/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "jleAseprite.h"
#include "jleGameEngine.h"
#include "jleResource.h"
#include <plog/Log.h>

#include <filesystem>
#include <fstream>

// jleAsepriteMetaLayer
void from_json(const nlohmann::json &j, jleAsepriteMetaLayer &a) {
    a.name = j.at("name");
    a.opacity = j.at("opacity");
    a.blendMode = j.at("blendMode");
}

// jleAsepriteMeta
void from_json(const nlohmann::json &j, jleAsepriteMeta &a) {
    a.app = j.at("app");
    a.version = j.at("version");
    a.image = j.at("image");
    a.format = j.at("format");
    a.size = j.at("size");
    a.scale = j.at("scale");
    a.layers = j.at("layers").get<std::vector<jleAsepriteMetaLayer>>();
}

// jleAsepriteFrame
void from_json(const nlohmann::json &j, jleAsepriteFrame &a) {
    a.frame = j.at("frame");
    a.rotated = j.at("rotated");
    a.trimmed = j.at("trimmed");
    a.spriteSourceSize = j.at("spriteSourceSize");
    a.sourceSize = j.at("sourceSize");
    a.duration = j.at("duration");
}

// jleAsepriteXY
void from_json(const nlohmann::json &j, jleAsepriteXY &a) {
    a.w = j.at("w");
    a.h = j.at("h");
}

// jleAsepriteXYWH
void from_json(const nlohmann::json &j, jleAsepriteXYWH &a) {
    a.x = j.at("x");
    a.y = j.at("y");
    a.w = j.at("w");
    a.h = j.at("h");
}

// jleAseprite
void from_json(const nlohmann::json &j, jleAseprite &a) {
    a.meta = j.at("meta");
    a.frames = j.at("frames").get<std::vector<jleAsepriteFrame>>();
}

bool jleAseprite::loadFromFile(const jlePath &path) {
    this->path = path.getRealPath();
    std::ifstream i(path.getRealPath());
    if (i.good()) {
        nlohmann::json j;
        i >> j;

        from_json(j, *this);
        loadImage();
        return true;
    }
    else {
        LOG_ERROR << "Could not load Aseprite json file " << path.getRealPath();
        return false;
    }
}

jleAseprite::jleAseprite(const jlePath &path) {
    this->path = path.getRealPath();
    loadFromFile(path);
}

void jleAseprite::loadImage() {
    std::filesystem::path p{path};
    p.remove_filename();
    imageTexture = gEngine->resources().loadResourceFromFile<jleTexture>(jlePath{p.string() + meta.image});
}

int jleAseprite::totalAnimationTimeMs() {
    int res = 0;
    for (auto &&frame : frames) {
        res += frame.duration;
    }
    return res;
}
