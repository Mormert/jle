#pragma once

#include "OpenGLFrameBuffer.h"
#include "iRenderingInternalAPI.h"
#include "iWindowInitializer.h"

#include "OpenGLFullscreenRendering.h"

class OpenGLRenderingAPI : public iRenderingInternalAPI
{
public:
	~OpenGLRenderingAPI(){}

	void Setup(std::unique_ptr<iQuadRendering> quads) override;

	void SetViewportDimensions(int x, int y, unsigned int width, unsigned int height) override;
	void Render(iFramebuffer& framebufferOut, unsigned int width, unsigned int height) override;
};

