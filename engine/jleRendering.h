// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleCamera.h"
#include "jleFrameBuffer_OpenGL.h"
#include "jleQuadRendering.h"
#include "jleTextRendering.h"
#include "jleWindowInitializerInterface.h"
#include <memory>

class jleRenderingFactoryInterface;

class jleRendering {
public:
    ~jleRendering() = default;

    void Setup(const jleRenderingFactoryInterface& renderFactory);

    void SetViewportDimensions(int x,
                               int y,
                               unsigned int width,
                               unsigned int height);

    void Render(jleFramebufferInterface& framebufferOut, jleCamera& camera);

    void ClearBuffersForNextFrame();

    jleQuadRendering& quads();
    jleTextRendering& texts();

private:
    std::unique_ptr<jleQuadRendering> _quads;
    std::unique_ptr<jleTextRendering> _texts;
};
