// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleAseprite.h"
#include "jleComponent.h"
#include "jleFrameBufferInterface.h"
#include "jleTransform.h"

class cCamera : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cCamera)
public:
    explicit cCamera(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_perspective),
           CEREAL_NVP(_farPlane),
           CEREAL_NVP(_nearPlane),
           CEREAL_NVP(_perspectiveFov),
           CEREAL_NVP(_framebufferSizeX),
           CEREAL_NVP(_framebufferSizeY),
           //CEREAL_NVP(_framebufferFixedAxis),
           CEREAL_NVP(_framebufferUseFixedAxis),
           CEREAL_NVP(_matchFramebufferToWindowSize));
    }

    ~cCamera() override;

    void start() override;

    void update(float dt) override;

    void framebufferResizeCallback(unsigned int width, unsigned int height);

protected:
    float _perspectiveFov{90.f};
    float _farPlane{10000.f};
    float _nearPlane{0.1f};
    int _framebufferSizeX{1024};
    int _framebufferSizeY{1024};
    bool _framebufferUseFixedAxis{false};
    bool _matchFramebufferToWindowSize{false};
    jleFramebufferInterface::FIXED_AXIS _framebufferFixedAxis{jleFramebufferInterface::FIXED_AXIS::width};

    bool _perspective{false};

    int _framebufferCallbackId;

    inline static uint32_t sInstanceCounter = 0;
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cCamera)

CEREAL_REGISTER_TYPE(cCamera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cCamera)