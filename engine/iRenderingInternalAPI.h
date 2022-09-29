// Copyright (c) 2022. Johan Lind

#pragma once

#include "iRenderingFactory.h"
#include "iFramebuffer.h"
#include "jleCamera.h"

namespace jle {
    class iRenderingInternalAPI : public iRenderingAPI {
    public:
        ~iRenderingInternalAPI() override = default;;

        virtual void SetViewportDimensions(int x, int y, unsigned int width, unsigned int height) = 0; // glviewport...

        virtual void Setup(const iRenderingFactory &renderFactory) = 0;

        virtual void Render(iFramebuffer &framebufferOut, jleCamera &camera) = 0;

        virtual void ClearBuffersForNextFrame() = 0;
    };
}

