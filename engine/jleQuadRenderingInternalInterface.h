// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleCamera.h"
#include "jleFramebufferInterface.h"
#include "jleQuadRenderingInterface.h"

#include <vector>

class jleQuadRenderingInternalInterface : public jleQuadRenderingInterface {

public:
    ~jleQuadRenderingInternalInterface() override = default;

    // Renders the queued quads to an output framebuffer
    virtual void QueueRender(jleFramebufferInterface& framebufferOut,
                             jleCamera& camera) = 0;

    // Renders quads passed to the render function to an output framebuffer
    virtual void Render(
        jleFramebufferInterface& framebufferOut,
        jleCamera& camera,
        const std::vector<TexturedQuad>& texturedQuads,
        const std::vector<TexturedHeightQuad>& texturedHeightQuads,
        bool clearDepthColor) = 0;

    virtual void ClearBuffersForNextFrame() = 0;
};
