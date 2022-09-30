// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFramebufferInterface.h"

class jleFullscreenRenderingInterface {
public:
    virtual ~jleFullscreenRenderingInterface() = default;

    virtual void RenderFramebufferFullscreen(
        jleFramebufferInterface& framebuffer,
        unsigned int screenWidth,
        unsigned int screenHeight) = 0;
};
