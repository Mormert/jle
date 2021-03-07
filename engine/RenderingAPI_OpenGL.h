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

		virtual void Setup(std::unique_ptr<iQuadRendering> quads) override;

		virtual void SetViewportDimensions(int x, int y, unsigned int width, unsigned int height) override;
		virtual void Render(iFramebuffer& framebufferOut) override;
	};
}