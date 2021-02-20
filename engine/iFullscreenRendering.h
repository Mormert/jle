#pragma once

#include "iFramebuffer.h"

class iFullscreenRendering
{
public:
	virtual ~iFullscreenRendering(){}

	virtual void RenderFramebufferFullscreen(iFramebuffer& framebuffer, unsigned int screenWidth, unsigned int screenHeight) = 0;
};