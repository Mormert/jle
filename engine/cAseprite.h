#pragma once

#include "jleComponent.h"
#include "jleAseprite.h"
#include "cTransform.h"

namespace jle {

    class cAseprite : public jle::jleComponent {
        JLE_REGISTER_COMPONENT_TYPE(cAseprite)
    public:
        explicit cAseprite(jle::jleObject *owner = nullptr, jle::jleScene *scene = nullptr);

        void Start() override;

        void Update(float dt) override;

        void ToJson(nlohmann::json &j_out) override;

        void FromJson(const nlohmann::json &j_in) override;

        std::string mAsepritePath;

        float mDepth{};
        uint32_t mWidth{}, mHeight{}, mTextureX{}, mTextureY{};

        bool mAnimating = false;

    private:
        std::shared_ptr<jleAseprite> mAseprite{nullptr};

        std::shared_ptr<cTransform> mTransform{nullptr};

        uint32_t mCurrentFrame = 0;
        uint32_t mCurrentFrameDurationMs{};
        float mCurrentFrameTimeSpent = 0.f;

    };

}