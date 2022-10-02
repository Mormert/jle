// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFramebufferInterface.h"
#include "jleFullscreenRenderingInterface.h"
#include "jleQuadRendering.h"
#include "jleRendering.h"
#include "jleTextRendering.h"

#include <memory>

class jleRenderingFactoryInterface {
public:
    virtual ~jleRenderingFactoryInterface() = default;

    virtual std::unique_ptr<jleRendering> CreateRenderingAPI()
        const = 0;

    virtual std::unique_ptr<jleQuadRendering> CreateQuadRendering()
        const = 0;

    virtual std::unique_ptr<jleTextRendering> CreateTextRendering()
        const = 0;

    virtual std::unique_ptr<jleFullscreenRenderingInterface>
    CreateFullscreenRendering() const = 0;

    virtual std::unique_ptr<jleFramebufferInterface> CreateFramebuffer(
        unsigned int width, unsigned int height) const = 0;
};
