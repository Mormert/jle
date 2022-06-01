// Copyright (c) 2022. Johan Lind

#pragma once


#include "jleComponent.h"
#include "jleAseprite.h"
#include "cTransform.h"

namespace jle {

    class cCamera : public jle::jleComponent {
        JLE_REGISTER_COMPONENT_TYPE(cCamera)
    public:
        explicit cCamera(jle::jleObject *owner = nullptr, jle::jleScene *scene = nullptr);

        ~cCamera() override;

        void Start() override;

        void ToJson(nlohmann::json &j_out) override;

        void FromJson(const nlohmann::json &j_in) override;

        void Update(float dt) override;

    private:
        std::shared_ptr<cTransform> mTransform{nullptr};
        float mOffsetX{};
        float mOffsetY{};

        inline static uint32_t sInstanceCounter = 0;
    };

}

