// Copyright (c) 2022. Johan Lind

#include "jleAseprite.h"
#include "jleGameEngine.h"
#include "plog/Log.h"

#include <filesystem>
#include <fstream>

// jleAsepriteMetaLayer
void from_json(const nlohmann::json& j, jleAsepriteMetaLayer& a) {
    a._name = j.at("name");
    a._opacity = j.at("opacity");
    a._blendmode = j.at("blendMode");
}

// jleAsepriteMeta
void from_json(const nlohmann::json& j, jleAsepriteMeta& a) {
    a._app = j.at("app");
    a._version = j.at("version");
    a._image = j.at("image");
    a._format = j.at("format");
    a._size = j.at("size");
    a._scale = j.at("scale");
    a._layers = j.at("layers").get<std::vector<jleAsepriteMetaLayer>>();
}

// jleAsepriteFrame
void from_json(const nlohmann::json& j, jleAsepriteFrame& a) {
    a._frame = j.at("frame");
    a._rotated = j.at("rotated");
    a._trimmed = j.at("trimmed");
    a._spriteSourceSize = j.at("spriteSourceSize");
    a._sourceSize = j.at("sourceSize");
    a._duration = j.at("duration");
}

// jleAsepriteXY
void from_json(const nlohmann::json& j, jleAsepriteXY& a) {
    a._w = j.at("w");
    a._h = j.at("h");
}

// jleAsepriteXYWH
void from_json(const nlohmann::json& j, jleAsepriteXYWH& a) {
    a._x = j.at("x");
    a._y = j.at("y");
    a._w = j.at("w");
    a._h = j.at("h");
}

// jleAseprite
void from_json(const nlohmann::json& j, jleAseprite& a) {
    a._meta = j.at("meta");
    a._frames = j.at("frames").get<std::vector<jleAsepriteFrame>>();
}

bool jleAseprite::loadFromFile(const std::string& path) {
    _path = path;
    std::ifstream i(path);
    if (i.good()) {
        nlohmann::json j;
        i >> j;

        from_json(j, *this);
        loadImage();
        return true;
    }
    else {
        LOG_ERROR << "Could not load Aseprite json file " << path;
        return false;
    }
}

jleAseprite::jleAseprite(const std::string& path) {
    _path = path;
    loadFromFile(path);
}

void jleAseprite::loadImage() {
    std::filesystem::path p{_path};
    p.remove_filename();
    _imageTexture =
        jleTexture::fromPath(jleRelativePath{p.string() + _meta._image});
}

int jleAseprite::totalAnimationTimeMs() {
    int res = 0;
    for (auto&& frame : _frames) {
        res += frame._duration;
    }
    return res;
}
