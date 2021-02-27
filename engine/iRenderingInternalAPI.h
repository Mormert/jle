#pragma once

#include "iRenderingAPI.h"
#include "iFramebuffer.h"

namespace jle
{
	class iRenderingInternalAPI : public iRenderingAPI
	{
	public:
		virtual ~iRenderingInternalAPI() {};
		virtual void SetViewportDimensions(int x, int y, unsigned int width, unsigned int height) = 0; // glviewport...

		virtual void Setup(std::unique_ptr<iQuadRendering> quads) = 0;

		virtual void Render(iFramebuffer& framebufferOut, unsigned int width, unsigned int height) = 0;
	};
}

