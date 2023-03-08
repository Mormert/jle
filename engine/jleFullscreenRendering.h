// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleShader.h"

class jleFramebufferInterface;

class jleFullscreenRendering {
public:
    jleFullscreenRendering(const jleFullscreenRendering &) = delete;
    jleFullscreenRendering &operator=(const jleFullscreenRendering &) = delete;
    jleFullscreenRendering(jleFullscreenRendering &&e) = delete;
    jleFullscreenRendering &operator=(jleFullscreenRendering &&e) = delete;

    jleFullscreenRendering();

    ~jleFullscreenRendering();

    void renderFramebufferFullscreen(jleFramebufferInterface &framebuffer,
                                     unsigned int screenWidth,
                                     unsigned int screenHeight);

private:
    jleShader quadScreenShader;
    unsigned int quadVAO, quadVBO;
};
