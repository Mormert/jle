#include "cAseprite.h"
#include "jleResourceHolder.h"
#include "jlePathDefines.h"
#include "jleObject.h"
#include "iQuadRendering.h"
#include "jleCore.h"

namespace jle {

    void cAseprite::Start() {
        mTransform = mAttachedToObject->GetComponent<cTransform>();
        if (!mTransform) {
            mTransform = mAttachedToObject->AddCustomComponent<cTransform>();
        }
    }

    void cAseprite::Update(float dt) {
        if (!mAseprite) {
            return;
        }

        if (mAnimating) {
            mCurrentFrameTimeSpent += dt * 1000.f;
            if (mCurrentFrameTimeSpent >= mCurrentFrameDurationMs) {
                mCurrentFrame++;
                if (mCurrentFrame == mAseprite->mFrames.size()) {
                    mCurrentFrame = 0;
                }
                mCurrentFrameTimeSpent = 0;
                mCurrentFrameDurationMs = mAseprite->mFrames.at(mCurrentFrame).mDuration;
            }
        }

        const auto& frame = mAseprite->mFrames.at(mCurrentFrame);

        auto &texture = mAseprite->mImageTexture;
        if (texture != nullptr) {
            TexturedQuad quad{texture};
            quad.x = mTransform->x;
            quad.y = mTransform->y;
            quad.height = frame.mFrame.mH;
            quad.width = frame.mFrame.mW;
            quad.textureX = frame.mFrame.mX;
            quad.textureY = frame.mFrame.mY;
            quad.depth = mDepth;

            if (quad.texture.get()) {
                jle::jleCore::core->rendering->quads->SendTexturedQuad(quad, RenderingMethod::Dynamic);
            }
        }
    }

    void cAseprite::ToJson(nlohmann::json &j_out) {
        j_out = nlohmann::json{
                {"asepritePath", mAsepritePath},
                {"depth",        mDepth},
                {"height",       mHeight},
                {"width",        mWidth},
                {"textureX",     mTextureX},
                {"textureY",     mTextureY},
                {"animating",    mAnimating}
        };
    }

    void cAseprite::FromJson(const nlohmann::json &j_in) {
        mAsepritePath = j_in.at("asepritePath");
        mDepth = j_in.at("depth");
        mHeight = j_in.at("height");
        mWidth = j_in.at("width");
        mTextureX = j_in.at("textureX");
        mTextureY = j_in.at("textureY");
        mAnimating = j_in.at("animating");

        const auto truePath = jle::FindTrueResourcePath(jleRootFolder::GameResources, mAsepritePath);
        mAseprite = jleResourceHolder<jleAseprite>::LoadResourceFromFile(truePath);
    }

    cAseprite::cAseprite(jle::jleObject *owner, jle::jleScene *scene) : jleComponent(owner, scene) {}

}