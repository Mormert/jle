// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFramebufferInterface.h"
#include "jleFullscreenRenderingInterface.h"
#include "jleQuadRenderingInterface.h"
#include "jleRenderingAPIInterface.h"

#include <memory>

class jleRenderingFactoryInterface {
public:
    virtual ~jleRenderingFactoryInterface() = default;

    virtual std::unique_ptr<jleRenderingAPIInterface> CreateRenderingAPI()
        const = 0;

    virtual std::unique_ptr<jleQuadRenderingInterface> CreateQuadRendering()
        const = 0;

    virtual std::unique_ptr<jleTextRenderingInterface> CreateTextRendering()
        const = 0;

    virtual std::unique_ptr<jleFullscreenRenderingInterface>
    CreateFullscreenRendering() const = 0;

    virtual std::unique_ptr<jleFramebufferInterface> CreateFramebuffer(
        unsigned int width, unsigned int height) const = 0;
};
