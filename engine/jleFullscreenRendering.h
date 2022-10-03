// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleShader.h"

class jleFramebuffer;

class jleFullscreenRendering {
public:
    jleFullscreenRendering(const jleFullscreenRendering&) =
        delete;
    jleFullscreenRendering& operator=(
        const jleFullscreenRendering&) = delete;
    jleFullscreenRendering(jleFullscreenRendering&& e) = delete;
    jleFullscreenRendering& operator=(
        jleFullscreenRendering&& e) = delete;

    jleFullscreenRendering();

    ~jleFullscreenRendering();

    void RenderFramebufferFullscreen(jleFramebuffer& framebuffer,
                                     unsigned int screenWidth,
                                     unsigned int screenHeight);

private:
    jleShader quadScreenShader;
    unsigned int quadVAO, quadVBO;
};
