// Copyright (c) 2022. Johan Lind

#ifndef HEXABLO_CUITRANSFORMUPDATER_H
#define HEXABLO_CUITRANSFORMUPDATER_H

#include "cCamera.h"
#include "cTransform.h"
#include "jleComponent.h"
#include "jleGame.h"

class cUITransformUpdater : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cUITransformUpdater)
public:
    explicit cUITransformUpdater(jleObject *owner = nullptr,
                                 jleScene *scene = nullptr);

    ~cUITransformUpdater() override;

    void Start() override;

    void ToJson(nlohmann::json& j_out) override;

    void FromJson(const nlohmann::json& j_in) override;

    void Update(float dt) override;

private:
    std::shared_ptr<cTransform> mTransform{nullptr};
    std::weak_ptr<cCamera> mCamera{};

    bool mTop{true};
    bool mBottom{false};
    bool mLeft{false};
    bool mRight{false};

    int mX{}, mY{};
    jleGame& mGameRef;
};

#endif // HEXABLO_CUITRANSFORMUPDATER_H
