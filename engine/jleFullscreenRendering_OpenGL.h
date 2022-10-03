// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleShader.h"

class Framebuffer_OpenGL;

class jleFullscreenRendering_OpenGL {
public:
    jleFullscreenRendering_OpenGL(const jleFullscreenRendering_OpenGL&) =
        delete;
    jleFullscreenRendering_OpenGL& operator=(
        const jleFullscreenRendering_OpenGL&) = delete;
    jleFullscreenRendering_OpenGL(jleFullscreenRendering_OpenGL&& e) = delete;
    jleFullscreenRendering_OpenGL& operator=(
        jleFullscreenRendering_OpenGL&& e) = delete;

    jleFullscreenRendering_OpenGL();

    ~jleFullscreenRendering_OpenGL();

    void RenderFramebufferFullscreen(Framebuffer_OpenGL& framebuffer,
                                     unsigned int screenWidth,
                                     unsigned int screenHeight);

private:
    jleShader quadScreenShader;
    unsigned int quadVAO, quadVBO;
};
