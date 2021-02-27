#pragma once

#include "iFramebufferFullscreenRenderer.h"

#include "Shader_OpenGL.h"

class FramebufferFullscreenRenderer_OpenGL : iFramebufferFullscreenRenderer
{
public:
	virtual ~FramebufferFullscreenRenderer_OpenGL();

	FramebufferFullscreenRenderer_OpenGL();

	// Inherited via iFramebufferFullscreenRenderer
	virtual void RenderFramebufferFullscreen(iFramebuffer& framebuffer, unsigned int screenWidth, unsigned int screenHeight) override;

private:
	jle::gfx::Shader_OpenGL quadScreenShader;
	unsigned int quadVAO, quadVBO;
};