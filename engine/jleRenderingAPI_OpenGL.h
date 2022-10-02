// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleCamera.h"
#include "jleFrameBuffer_OpenGL.h"
#include "jleQuadRendering_OpenGL.h"
#include "jleTextRendering_OpenGL.h"
#include "jleWindowInitializerInterface.h"
#include <memory>

class jleRenderingFactoryInterface;

class jleRenderingAPI_OpenGL {
public:
    ~jleRenderingAPI_OpenGL() = default;

    void Setup(const jleRenderingFactoryInterface& renderFactory);

    void SetViewportDimensions(int x,
                               int y,
                               unsigned int width,
                               unsigned int height);

    void Render(jleFramebufferInterface& framebufferOut, jleCamera& camera);

    void ClearBuffersForNextFrame();

    jleQuadRendering_OpenGL& quads();
    jleTextRendering_OpenGL& texts();

private:
    std::unique_ptr<jleQuadRendering_OpenGL> _quads;
    std::unique_ptr<jleTextRendering_OpenGL> _texts;
};
