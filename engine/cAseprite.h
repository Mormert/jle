// Copyright (c) 2022. Johan Lind

#pragma once

#include "cTransform.h"
#include "jleAseprite.h"
#include "jleComponent.h"

class cAseprite : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cAseprite)
public:
    explicit cAseprite(jleObject *owner = nullptr, jleScene *scene = nullptr);

    void Start() override;

    void Update(float dt) override;

    void ToJson(nlohmann::json& j_out) override;

    void FromJson(const nlohmann::json& j_in) override;

    int AddAsepritePath(const std::string& path);

    void SetCurrentAseprite(unsigned int index);

    void SetCurrentAsepriteFrame(unsigned int index);

    [[nodiscard]] unsigned int GetCurrentAsepriteIndex() const;

    [[nodiscard]] jleAseprite& GetActiveAsepriteRef();

    std::vector<jleJsonString> mAsepritePaths;

    uint32_t mWidth{}, mHeight{}, mTextureX{}, mTextureY{};
    int32_t mOffsetX{}, mOffsetY{};

    bool mAnimating = false;

private:
    unsigned int mCurrentlyActiveAseprite{0};
    std::vector<std::shared_ptr<jleAseprite>> mAseprites{};

    std::shared_ptr<cTransform> mTransform{nullptr};

    uint32_t mCurrentFrame = 0;
    uint32_t mCurrentFrameDurationMs{};
    float mCurrentFrameTimeSpent = 0.f;
};
