// Copyright (c) 2022. Johan Lind

#include "cAseprite.h"
#include "jleResourceHolder.h"
#include "jlePathDefines.h"
#include "jlePath.h"
#include "jleObject.h"
#include "iQuadRendering.h"
#include "jleGameEngine.h"

namespace jle {

    void cAseprite::Start() {
        mTransform = mAttachedToObject->AddDependencyComponent<cTransform>(this);

        mAseprites.clear();
        for (auto &&path: mAsepritePaths) {
            mAseprites.push_back(jleResourceHolder::LoadResourceFromFile<jleAseprite>(jleRelativePath{path.mString}));
        }
    }

    void cAseprite::Update(float dt) {
        if (mAseprites.empty() || mCurrentlyActiveAseprite + 1 > mAseprites.size()) {
            return;
        }

        auto &&aseprite = mAseprites[mCurrentlyActiveAseprite];

        if (aseprite->mFrames.empty()) {
            return;
        }

        if (mAnimating) {
            mCurrentFrameTimeSpent += dt * 1000.f;
            if (mCurrentFrameTimeSpent >= mCurrentFrameDurationMs) {
                mCurrentFrame++;
                if (mCurrentFrame >= aseprite->mFrames.size()) {
                    mCurrentFrame = 0;
                }
                mCurrentFrameTimeSpent = 0;
                mCurrentFrameDurationMs = aseprite->mFrames.at(mCurrentFrame).mDuration;
            }
        }

        if(mCurrentFrame >= aseprite->mFrames.size())
        {
            mCurrentFrame = aseprite->mFrames.size() - 1;
        }
        const auto &frame = aseprite->mFrames.at(mCurrentFrame);

        auto &texture = aseprite->mImageTexture;
        if (texture != nullptr) {
            TexturedQuad quad{texture};
            quad.x = mTransform->GetWorldX() + mOffsetX;
            quad.y = mTransform->GetWorldY() + mOffsetY;
            quad.height = mHeight;
            quad.width = mWidth;
            quad.textureX = frame.mFrame.mX + mTextureX;
            quad.textureY = frame.mFrame.mY + mTextureY;
            quad.depth = mTransform->GetWorldDepth();

            if (quad.texture.get()) {
                jle::jleCore::core->rendering->quads->SendTexturedQuad(quad, RenderingMethod::Dynamic);
            }
        }
    }

    void cAseprite::ToJson(nlohmann::json &j_out) {
        j_out = nlohmann::json{
                {"mAsepritePaths_STRVEC", mAsepritePaths},
                {"height",                mHeight},
                {"width",                 mWidth},
                {"textureX",              mTextureX},
                {"textureY",              mTextureY},
                {"offsetX",               mOffsetX},
                {"offsetY",               mOffsetY},
                {"animating",             mAnimating}
        };
    }

    void cAseprite::FromJson(const nlohmann::json &j_in) {


        const auto asepritePaths = j_in.find("mAsepritePaths_STRVEC");
        if (asepritePaths != j_in.end()) {
            mAsepritePaths = j_in.at("mAsepritePaths_STRVEC").get<std::vector<jleJsonString>>();
        }

        mHeight = j_in.at("height");
        mWidth = j_in.at("width");
        mTextureX = j_in.at("textureX");
        mTextureY = j_in.at("textureY");
        mOffsetX = j_in.at("offsetX");
        mOffsetY = j_in.at("offsetY");
        mAnimating = j_in.at("animating");

        // Make sure to reset current frame to not cause out of bounds crash
        mCurrentFrame = 0;
        mCurrentFrameTimeSpent = 0.f;
        mCurrentlyActiveAseprite = 0;

        mAseprites.clear();
        for (auto &&path: mAsepritePaths) {
            mAseprites.push_back(jleResourceHolder::LoadResourceFromFile<jleAseprite>(jleRelativePath{path.mString}));
        }
    }

    cAseprite::cAseprite(jle::jleObject *owner, jle::jleScene *scene) : jleComponent(owner, scene) {}

    void cAseprite::SetCurrentAseprite(unsigned int index) {
        if (index < mAseprites.size()) {
            mCurrentlyActiveAseprite = index;
            return;
        }
        LOGE << "Trying to set active aseprite animation outside bounds!";
    }

    unsigned int cAseprite::GetCurrentAsepriteIndex() const {
        return mCurrentlyActiveAseprite;
    }

    int cAseprite::AddAsepritePath(const std::string &path) {
        mAsepritePaths.push_back({path});
        mAseprites.push_back(jleResourceHolder::LoadResourceFromFile<jleAseprite>(jleRelativePath{path}));
        return (int) mAseprites.size() - 1;
    }

    jleAseprite &cAseprite::GetActiveAsepriteRef() {
        return *mAseprites[mCurrentlyActiveAseprite];
    }

    void cAseprite::SetCurrentAsepriteFrame(unsigned int index) {
        if(mAseprites.empty())
        {
            LOGE << "No found aseprites on this object";
            return;
        }

        const auto frames = mAseprites[mCurrentlyActiveAseprite]->mFrames.size();
        if (index < frames) {
            mCurrentFrameDurationMs = 0;
            mCurrentFrame = index;
            return;
        }
        LOGE << "Trying to set current frame outside bounds!";
    }

}