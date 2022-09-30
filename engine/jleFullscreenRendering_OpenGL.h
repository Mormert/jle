// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFullscreenRenderingInterface.h"

#include "no_copy_no_move.h"

#include "jleShader.h"

class jleFullscreenRendering_OpenGL : public jleFullscreenRenderingInterface {
public:
    NO_COPY_NO_MOVE(jleFullscreenRendering_OpenGL)

    jleFullscreenRendering_OpenGL();

    virtual ~jleFullscreenRendering_OpenGL();

    virtual void RenderFramebufferFullscreen(
        jleFramebufferInterface& framebuffer,
        unsigned int screenWidth,
        unsigned int screenHeight) override;

private:
    jleShader quadScreenShader;
    unsigned int quadVAO, quadVBO;
};
