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

		void Setup(std::unique_ptr<iQuadRendering> quads) override;

		void SetViewportDimensions(int x, int y, unsigned int width, unsigned int height) override;
		void Render(iFramebuffer& framebufferOut, unsigned int width, unsigned int height) override;
	};
}