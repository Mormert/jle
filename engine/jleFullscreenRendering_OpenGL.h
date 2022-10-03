// Copyright (c) 2022. Johan Lind

#pragma once

#include "no_copy_no_move.h"

#include "jleShader.h"

class Framebuffer_OpenGL;

class jleFullscreenRendering_OpenGL {
public:
    NO_COPY_NO_MOVE(jleFullscreenRendering_OpenGL)

    jleFullscreenRendering_OpenGL();

    ~jleFullscreenRendering_OpenGL();

    void RenderFramebufferFullscreen(Framebuffer_OpenGL& framebuffer,
                                     unsigned int screenWidth,
                                     unsigned int screenHeight);

private:
    jleShader quadScreenShader;
    unsigned int quadVAO, quadVBO;
};
