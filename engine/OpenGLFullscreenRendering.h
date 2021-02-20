#pragma once

#include "iFullscreenRendering.h"

#include "no_copy_no_move.h"

#include "Shader.h"

class OpenGLFullscreenRendering : public iFullscreenRendering 
{
public:
	NO_COPY_NO_MOVE(OpenGLFullscreenRendering)

	OpenGLFullscreenRendering();
	virtual ~OpenGLFullscreenRendering();

	virtual void RenderFramebufferFullscreen(iFramebuffer& framebuffer, unsigned int screenWidth, unsigned int screenHeight) override;

private:
	jle::internals::Shader quadScreenShader;
	unsigned int quadVAO, quadVBO;
};

