// Copyright (c) 2022. Johan Lind

#pragma once

#include "FrameBuffer_OpenGL.h"
#include "iRenderingInternalAPI.h"
#include "iWindowInitializer.h"

#include "FullscreenRendering_OpenGL.h"

namespace jle {
    class RenderingAPI_OpenGL : public iRenderingInternalAPI {
    public:
        ~RenderingAPI_OpenGL() override = default;

        void Setup(const iRenderingFactory &renderFactory) override;

        void SetViewportDimensions(int x, int y, unsigned int width, unsigned int height) override;

        void Render(iFramebuffer &framebufferOut, const jleCamera &camera) override;

        void ClearBuffersForNextFrame() override;
    };
}