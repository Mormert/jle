#pragma once

#include "iFramebuffer.h"

namespace jle
{
	class iFramebufferFullscreenRenderer
	{
	public:
		virtual ~iFramebufferFullscreenRenderer() {}

		virtual void RenderFramebufferFullscreen(iFramebuffer& framebuffer, unsigned int screenWidth, unsigned int screenHeight) = 0;
	};

}