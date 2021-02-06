#pragma once

#include "iFramebufferFullscreenRenderer.h"

#include "Shader.h"

class OpenGLFramebufferFullscreenRenderer : iFramebufferFullscreenRenderer
{
public:
	virtual ~OpenGLFramebufferFullscreenRenderer();

	OpenGLFramebufferFullscreenRenderer();

	// Inherited via iFramebufferFullscreenRenderer
	virtual void RenderFramebufferFullscreen(iFramebuffer& framebuffer, unsigned int screenWidth, unsigned int screenHeight) override;

private:
	jle::internals::Shader quadScreenShader;
	unsigned int quadVAO, quadVBO;
};