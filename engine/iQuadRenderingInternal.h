// Copyright (c) 2022. Johan Lind

#pragma once

#include "iQuadRendering.h"
#include "jleCamera.h"
#include "iFramebuffer.h"

namespace jle {
    class iQuadRenderingInternal : public iQuadRendering {

    public:
        ~iQuadRenderingInternal() override = default;

        // Renders the queued quads to an output framebuffer
        virtual void QueueRender(iFramebuffer &framebufferOut, const jleCamera &camera) = 0;

        // Renders quads passed to the render function to an output framebuffer
        virtual void Render(iFramebuffer &framebufferOut, const jleCamera &camera, const std::vector<TexturedQuad>& texturedQuads, bool clearDepthColor) = 0;

        virtual void ClearBuffersForNextFrame() = 0;
    };
}