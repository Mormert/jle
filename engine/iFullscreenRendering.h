// Copyright (c) 2022. Johan Lind

#pragma once

#include "iFramebuffer.h"

namespace jle {
    class iFullscreenRendering {
    public:
        virtual ~iFullscreenRendering() = default;

        virtual void
        RenderFramebufferFullscreen(iFramebuffer &framebuffer, unsigned int screenWidth, unsigned int screenHeight) = 0;
    };
}