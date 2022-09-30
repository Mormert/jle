// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFrameBuffer_OpenGL.h"
#include "jleRenderingInternalAPIInterface.h"
#include "jleWindowInitializerInterface.h"

#include "jleFullscreenRendering_OpenGL.h"

class jleRenderingAPI_OpenGL : public jleRenderingInternalAPIInterface {
public:
    ~jleRenderingAPI_OpenGL() override = default;

    void Setup(const jleRenderingFactoryInterface& renderFactory) override;

    void SetViewportDimensions(int x,
                               int y,
                               unsigned int width,
                               unsigned int height) override;

    void Render(jleFramebufferInterface& framebufferOut, jleCamera& camera) override;

    void ClearBuffersForNextFrame() override;
};
