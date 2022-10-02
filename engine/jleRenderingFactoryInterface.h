// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFramebufferInterface.h"
#include "jleFullscreenRenderingInterface.h"
#include "jleQuadRendering_OpenGL.h"
#include "jleRenderingAPI_OpenGL.h"
#include "jleTextRendering_OpenGL.h"

#include <memory>

class jleRenderingFactoryInterface {
public:
    virtual ~jleRenderingFactoryInterface() = default;

    virtual std::unique_ptr<jleRenderingAPI_OpenGL> CreateRenderingAPI()
        const = 0;

    virtual std::unique_ptr<jleQuadRendering_OpenGL> CreateQuadRendering()
        const = 0;

    virtual std::unique_ptr<jleTextRendering_OpenGL> CreateTextRendering()
        const = 0;

    virtual std::unique_ptr<jleFullscreenRenderingInterface>
    CreateFullscreenRendering() const = 0;

    virtual std::unique_ptr<jleFramebufferInterface> CreateFramebuffer(
        unsigned int width, unsigned int height) const = 0;
};
