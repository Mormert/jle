// Copyright (c) 2022. Johan Lind

#include "jleAseprite.h"
#include "jleGameEngine.h"
#include "plog/Log.h"

#include <fstream>
#include <filesystem>

// jleAsepriteMetaLayer
void jle::from_json(const nlohmann::json &j, jleAsepriteMetaLayer &a) {
    a.mName = j.at("name");
    a.mOpacity = j.at("opacity");
    a.mBlendmode = j.at("blendMode");
}

// jleAsepriteMeta
void jle::from_json(const nlohmann::json &j, jleAsepriteMeta &a) {
    a.mApp = j.at("app");
    a.mVersion = j.at("version");
    a.mImage = j.at("image");
    a.mFormat = j.at("format");
    a.mSize = j.at("size");
    a.mScale = j.at("scale");
    a.mLayers = j.at("layers").get<std::vector<jleAsepriteMetaLayer>>();
}

// jleAsepriteFrame
void jle::from_json(const nlohmann::json &j, jleAsepriteFrame &a) {
    a.mFrame = j.at("frame");
    a.mRotated = j.at("rotated");
    a.mTrimmed = j.at("trimmed");
    a.mSpriteSourceSize = j.at("spriteSourceSize");
    a.mSourceSize = j.at("sourceSize");
    a.mDuration = j.at("duration");
}

// jleAsepriteXY
void jle::from_json(const nlohmann::json &j, jleAsepriteXY &a) {
    a.mW = j.at("w");
    a.mH = j.at("h");
}

// jleAsepriteXYWH
void jle::from_json(const nlohmann::json &j, jleAsepriteXYWH &a) {
    a.mX = j.at("x");
    a.mY = j.at("y");
    a.mW = j.at("w");
    a.mH = j.at("h");
}

// jleAseprite
void jle::from_json(const nlohmann::json &j, jleAseprite &a) {
    a.mMeta = j.at("meta");
    a.mFrames = j.at("frames").get<std::vector<jleAsepriteFrame>>();
}

bool jle::jleAseprite::LoadFromFile(const std::string &path) {
    mPath = path;
    std::ifstream i(path);
    if (i.good()) {
        nlohmann::json j;
        i >> j;

        jle::from_json(j, *this);
        LoadImage();
        return true;
    } else {
        LOG_ERROR << "Could not load Aseprite json file " << path;
        return false;
    }
}

jle::jleAseprite::jleAseprite(const std::string &path) {
    mPath = path;
    LoadFromFile(path);
}

void jle::jleAseprite::LoadImage() {
    std::filesystem::path p{mPath};
    p.remove_filename();
    mImageTexture = jleCore::core->texture_creator->LoadTextureFromPath(jleRelativePath{p.string() + mMeta.mImage});
}

int jle::jleAseprite::GetTotalAnimationTimeMs() {
    int res = 0;
    for (auto &&frame: mFrames) {
        res += frame.mDuration;
    }
    return res;
}
