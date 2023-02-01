// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFileLoadInterface.h"
#include "jleTexture.h"

#include "json.hpp"
#include <cereal/cereal.hpp>

#include <string>
#include <vector>

struct jleAsepriteXYWH {

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(w), CEREAL_NVP(h));
    }

    int32_t x, y, w, h;
};

struct jleAsepriteXY {

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(w), CEREAL_NVP(h));
    }

    int32_t w, h;
};

struct jleAsepriteFrame {

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(rotated),
           CEREAL_NVP(trimmed),
           CEREAL_NVP(duration),
           CEREAL_NVP(frame),
           CEREAL_NVP(spriteSourceSize),
           CEREAL_NVP(sourceSize));
    }

    bool rotated, trimmed;
    int32_t duration;
    jleAsepriteXYWH frame, spriteSourceSize;
    jleAsepriteXY sourceSize;
};

struct jleAsepriteMetaLayer {

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(name), CEREAL_NVP(blendMode), CEREAL_NVP(opacity));
    }

    std::string name, blendMode;
    int32_t opacity;
};

struct jleAsepriteMeta {

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(app),
           CEREAL_NVP(version),
           CEREAL_NVP(image),
           CEREAL_NVP(format),
           CEREAL_NVP(layers),
           CEREAL_NVP(size),
           CEREAL_NVP(scale));
    }

    std::string app;
    std::string version;
    std::string image;
    std::string format;
    std::vector<jleAsepriteMetaLayer> layers;
    jleAsepriteXY size;
    std::string scale;
};

struct jleAseprite : public jleFileLoadInterface {

    jleAseprite() = default;

    explicit jleAseprite(const std::string &path);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(meta), CEREAL_NVP(frames));
    }

    // Finds and sets the image resource reference for this jleAseprite
    void loadImage();

    bool loadFromFile(const std::string &path) override;

    // This format expects Aseprite exporting to use 'Array' and not 'Hash'
    std::vector<jleAsepriteFrame> frames;

    int totalAnimationTimeMs();

    jleAsepriteMeta meta;

    std::shared_ptr<jleTexture> imageTexture;
    std::string path;
};

void from_json(const nlohmann::json &j, jleAseprite &a);

void from_json(const nlohmann::json &j, jleAsepriteMeta &a);

void from_json(const nlohmann::json &j, jleAsepriteMetaLayer &a);

void from_json(const nlohmann::json &j, jleAsepriteFrame &a);

void from_json(const nlohmann::json &j, jleAsepriteXYWH &a);

void from_json(const nlohmann::json &j, jleAsepriteXY &a);
