// Copyright (c) 2022. Johan Lind

#include "jleRenderingFactory_OpenGL.h"

#include "jleFrameBuffer_OpenGL.h"
#include "jleFullscreenRendering_OpenGL.h"
#include "jleQuadRendering.h"
#include "jleRendering.h"
#include "jleTextRendering.h"
#include "plog/Log.h"

#include <memory>

std::unique_ptr<jleRendering> jleRenderingFactory_OpenGL::
    CreateRenderingAPI() const {
    return std::make_unique<jleRendering>();
}

std::unique_ptr<jleQuadRendering> jleRenderingFactory_OpenGL::
    CreateQuadRendering() const {
    LOG_VERBOSE << "Factory creating quad rendering";
    return std::make_unique<jleQuadRendering>();
}

std::unique_ptr<jleTextRendering> jleRenderingFactory_OpenGL::
    CreateTextRendering() const {
    return std::make_unique<jleTextRendering>();
}

std::unique_ptr<jleFramebufferInterface> jleRenderingFactory_OpenGL::
    CreateFramebuffer(unsigned int width, unsigned int height) const {
    return std::make_unique<Framebuffer_OpenGL>(width, height);
}

std::unique_ptr<jleFullscreenRenderingInterface> jleRenderingFactory_OpenGL::
    CreateFullscreenRendering() const {
    return std::make_unique<jleFullscreenRendering_OpenGL>();
}
