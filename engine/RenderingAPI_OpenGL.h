#pragma once

#include "FrameBuffer_OpenGL.h"
#include "iRenderingInternalAPI.h"
#include "iWindowInitializer.h"

#include "FullscreenRendering_OpenGL.h"

namespace jle
{
	class RenderingAPI_OpenGL : public iRenderingInternalAPI
	{
	public:
		~RenderingAPI_OpenGL() {}

		virtual void Setup(const iRenderingFactory& renderFactory) override;

		virtual void SetViewportDimensions(int x, int y, unsigned int width, unsigned int height) override;
		virtual void Render(iFramebuffer& framebufferOut, const jleCamera& camera) override;
        virtual void ClearBuffersForNextFrame() override;
	};
}