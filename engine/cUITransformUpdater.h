// Copyright (c) 2022. Johan Lind

#ifndef HEXABLO_CUITRANSFORMUPDATER_H
#define HEXABLO_CUITRANSFORMUPDATER_H

#include "jleGame.h"
#include "cCamera.h"
#include "jleComponent.h"
#include "cTransform.h"

namespace jle {

    class cUITransformUpdater : public jle::jleComponent {
        JLE_REGISTER_COMPONENT_TYPE(cUITransformUpdater)
    public:
        explicit cUITransformUpdater(jle::jleObject *owner = nullptr, jle::jleScene *scene = nullptr);

        ~cUITransformUpdater() override;

        void Start() override;

        void ToJson(nlohmann::json &j_out) override;

        void FromJson(const nlohmann::json &j_in) override;

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

}
#endif //HEXABLO_CUITRANSFORMUPDATER_H
