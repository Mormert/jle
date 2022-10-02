// Copyright (c) 2022. Johan Lind

#include "jleRenderingFactory_OpenGL.h"

#include "jleFrameBuffer_OpenGL.h"
#include "jleFullscreenRendering_OpenGL.h"
#include "jleQuadRendering_OpenGL.h"
#include "jleRenderingAPI_OpenGL.h"
#include "jleTextRendering_OpenGL.h"
#include "plog/Log.h"

#include <memory>

std::unique_ptr<jleRenderingAPI_OpenGL> jleRenderingFactory_OpenGL::
    CreateRenderingAPI() const {
    return std::make_unique<jleRenderingAPI_OpenGL>();
}

std::unique_ptr<jleQuadRendering_OpenGL> jleRenderingFactory_OpenGL::
    CreateQuadRendering() const {
    LOG_VERBOSE << "Factory creating quad rendering";
    return std::make_unique<jleQuadRendering_OpenGL>();
}

std::unique_ptr<jleTextRendering_OpenGL> jleRenderingFactory_OpenGL::
    CreateTextRendering() const {
    return std::make_unique<jleTextRendering_OpenGL>();
}

std::unique_ptr<jleFramebufferInterface> jleRenderingFactory_OpenGL::
    CreateFramebuffer(unsigned int width, unsigned int height) const {
    return std::make_unique<Framebuffer_OpenGL>(width, height);
}

std::unique_ptr<jleFullscreenRenderingInterface> jleRenderingFactory_OpenGL::
    CreateFullscreenRendering() const {
    return std::make_unique<jleFullscreenRendering_OpenGL>();
}
