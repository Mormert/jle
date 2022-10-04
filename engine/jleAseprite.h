// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFileLoadInterface.h"
#include "jleTexture.h"

#include "json.hpp"

#include <string>
#include <vector>

struct jleAsepriteXYWH {
    int32_t _x, _y, _w, _h;
};

struct jleAsepriteXY {
    int32_t _w, _h;
};

struct jleAsepriteFrame {
    bool _rotated, _trimmed;
    int32_t _duration;
    jleAsepriteXYWH _frame, _spriteSourceSize;
    jleAsepriteXY _sourceSize;
};

struct jleAsepriteMetaLayer {
    std::string _name, _blendmode;
    int32_t _opacity;
};

struct jleAsepriteMeta {
    std::string _app;
    std::string _version;
    std::string _image;
    std::string _format;
    std::vector<jleAsepriteMetaLayer> _layers;
    jleAsepriteXY _size;
    std::string _scale;
};

struct jleAseprite : jleFileLoadInterface {

    jleAseprite() = default;

    explicit jleAseprite(const std::string& path);

    // Finds and sets the image resource reference for this jleAseprite
    void loadImage();

    bool loadFromFile(const std::string& path) override;

    // This format expects Aseprite exporting to use 'Array' and not 'Hash'
    std::vector<jleAsepriteFrame> _frames;

    int totalAnimationTimeMs();

    jleAsepriteMeta _meta;

    std::shared_ptr<jleTexture> _imageTexture;
    std::string _path;
};

void from_json(const nlohmann::json& j, jleAseprite& a);

void from_json(const nlohmann::json& j, jleAsepriteMeta& a);

void from_json(const nlohmann::json& j, jleAsepriteMetaLayer& a);

void from_json(const nlohmann::json& j, jleAsepriteFrame& a);

void from_json(const nlohmann::json& j, jleAsepriteXYWH& a);

void from_json(const nlohmann::json& j, jleAsepriteXY& a);
