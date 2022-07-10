// Copyright (c) 2022. Johan Lind

#pragma once

#include "iTexture.h"
#include "FileLoadInterface.h"

#include "json.hpp"

#include <vector>
#include <string>

namespace jle {

    struct jleAsepriteXYWH {
        int32_t mX, mY, mW, mH;
    };

    struct jleAsepriteXY {
        int32_t mW, mH;
    };

    struct jleAsepriteFrame {
        bool mRotated, mTrimmed;
        int32_t mDuration;
        jleAsepriteXYWH mFrame, mSpriteSourceSize;
        jleAsepriteXY mSourceSize;
    };

    struct jleAsepriteMetaLayer {
        std::string mName, mBlendmode;
        int32_t mOpacity;
    };

    struct jleAsepriteMeta {
        std::string mApp;
        std::string mVersion;
        std::string mImage;
        std::string mFormat;
        std::vector<jleAsepriteMetaLayer> mLayers;
        jleAsepriteXY mSize;
        std::string mScale;
    };


    struct jleAseprite : FileLoadInterface {

        jleAseprite() = default;

        explicit jleAseprite(const std::string &path);

        // Finds and sets the image resource reference for this jleAseprite
        void LoadImage();

        bool LoadFromFile(const std::string &path) override;

        // This format expects Aseprite exporting to use 'Array' and not 'Hash'
        std::vector<jleAsepriteFrame> mFrames;

        int GetTotalAnimationTimeMs();

        jleAsepriteMeta mMeta;

        std::shared_ptr<jle::iTexture> mImageTexture;
        std::string mPath;
    };


    void from_json(const nlohmann::json &j, jleAseprite &a);

    void from_json(const nlohmann::json &j, jleAsepriteMeta &a);

    void from_json(const nlohmann::json &j, jleAsepriteMetaLayer &a);

    void from_json(const nlohmann::json &j, jleAsepriteFrame &a);

    void from_json(const nlohmann::json &j, jleAsepriteXYWH &a);

    void from_json(const nlohmann::json &j, jleAsepriteXY &a);

}
