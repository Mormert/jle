// Copyright (c) 2022. Johan Lind

#pragma once

#include "cTransform.h"
#include "jleAseprite.h"
#include "jleComponent.h"
#include "jleFrameBuffer.h"

class cCamera : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cCamera)
public:
    explicit cCamera(jleObject *owner = nullptr, jleScene *scene = nullptr);

    ~cCamera() override;

    void start() override;

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

    void update(float dt) override;

    void framebufferResizeCallback(unsigned int width, unsigned int height);

protected:
    std::shared_ptr<cTransform> _transform{nullptr};

    float _perspectiveFov{90.f};
    float _farPlane{10000.f};
    float _nearPlane{0.1f};
    int _framebufferSizeX{1024};
    int _framebufferSizeY{1024};
    bool _framebufferUseFixedAxis{false};
    bool _matchFramebufferToWindowSize{false};
    jleFramebuffer::FIXED_AXIS _framebufferFixedAxis{jleFramebuffer::FIXED_AXIS::width};

    bool _perspective{false};

    int _framebufferCallbackId;

    inline static uint32_t sInstanceCounter = 0;
};
