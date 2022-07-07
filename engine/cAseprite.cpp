// Copyright (c) 2022. Johan Lind

#include "cAseprite.h"
#include "jleResourceHolder.h"
#include "jlePathDefines.h"
#include "jleObject.h"
#include "iQuadRendering.h"
#include "jleCore.h"

namespace jle {

    void cAseprite::Start() {
        mTransform = mAttachedToObject->AddDependencyComponent<cTransform>(this);
        if(!mAsepritePath.empty())
        {
            const auto truePath = jle::FindTrueResourcePath(mAsepritePath);
            mAseprite = jleResourceHolder<jleAseprite>::LoadResourceFromFile(truePath);
        }
    }

    void cAseprite::Update(float dt) {
        if (!mAseprite) {
            return;
        }

        if(mAseprite->mFrames.empty())
        {
            return;
        }

        if (mAnimating) {
            mCurrentFrameTimeSpent += dt * 1000.f;
            if (mCurrentFrameTimeSpent >= mCurrentFrameDurationMs) {
                mCurrentFrame++;
                if (mCurrentFrame >= mAseprite->mFrames.size()) {
                    mCurrentFrame = 0;
                }
                mCurrentFrameTimeSpent = 0;
                mCurrentFrameDurationMs = mAseprite->mFrames.at(mCurrentFrame).mDuration;
            }
        }

        const auto &frame = mAseprite->mFrames.at(mCurrentFrame);

        auto &texture = mAseprite->mImageTexture;
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
                {"asepritePath", mAsepritePath},
                {"height",       mHeight},
                {"width",        mWidth},
                {"textureX",     mTextureX},
                {"textureY",     mTextureY},
                {"offsetX",     mOffsetX},
                {"offsetY",     mOffsetY},
                {"animating",    mAnimating}
        };
    }

    void cAseprite::FromJson(const nlohmann::json &j_in) {
        mAsepritePath = j_in.at("asepritePath");
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

       const auto truePath = jle::FindTrueResourcePath(mAsepritePath);
       mAseprite = jleResourceHolder<jleAseprite>::LoadResourceFromFile(truePath);
    }

    cAseprite::cAseprite(jle::jleObject *owner, jle::jleScene *scene) : jleComponent(owner, scene) {}

}