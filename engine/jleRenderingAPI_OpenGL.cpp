// Copyright (c) 2022. Johan Lind

#include "jleRenderingAPI_OpenGL.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <glad/glad.h>

#endif

#include "jleProfiler.h"
#include "jleQuadRendering_OpenGL.h"
#include "jleRenderingFactoryInterface.h"
#include "jleTextRendering_OpenGL.h"
#include "plog/Log.h"

void jleRenderingAPI_OpenGL::SetViewportDimensions(int x,
                                                   int y,
                                                   unsigned int width,
                                                   unsigned int height) {
    glViewport(x, y, static_cast<int>(width), static_cast<int>(height));
}

void jleRenderingAPI_OpenGL::Render(jleFramebufferInterface& framebufferOut,
                                    jleCamera& camera) {
    JLE_SCOPE_PROFILE(jleRenderingAPI_OpenGL::Render)
    _quads->QueueRender(framebufferOut, camera);
    _texts->Render(framebufferOut, camera);
}

void jleRenderingAPI_OpenGL::Setup(
    const jleRenderingFactoryInterface& renderFactory) {
    LOG_VERBOSE << "Creating text rendering";
    this->_texts = renderFactory.CreateTextRendering();
    LOG_VERBOSE << "Creating quad rendering";
    this->_quads = renderFactory.CreateQuadRendering();
}

void jleRenderingAPI_OpenGL::ClearBuffersForNextFrame() {
    _quads->ClearBuffersForNextFrame();
    _texts->ClearBuffersForNextFrame();
}

jleQuadRendering_OpenGL &jleRenderingAPI_OpenGL::quads() { return *_quads; }

jleTextRendering_OpenGL &jleRenderingAPI_OpenGL::texts() { return *_texts; }
