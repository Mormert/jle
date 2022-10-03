// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleCamera.h"
#include "jleFrameBuffer.h"
#include "jleQuadRendering.h"
#include "jleTextRendering.h"
#include <memory>

class jleFramebuffer;

class jleRendering {
public:
    ~jleRendering() = default;

    void Setup();

    void SetViewportDimensions(int x,
                               int y,
                               unsigned int width,
                               unsigned int height);

    void Render(jleFramebuffer& framebufferOut, jleCamera& camera);

    void ClearBuffersForNextFrame();

    jleQuadRendering& quads();
    jleTextRendering& texts();

private:
    std::unique_ptr<jleQuadRendering> _quads;
    std::unique_ptr<jleTextRendering> _texts;
};
