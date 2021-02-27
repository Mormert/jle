#pragma once

#include "iFullscreenRendering.h"

#include "no_copy_no_move.h"

#include "Shader_OpenGL.h"

namespace jle
{

	class FullscreenRendering_OpenGL : public iFullscreenRendering
	{
	public:
		NO_COPY_NO_MOVE(FullscreenRendering_OpenGL)

			FullscreenRendering_OpenGL();
		virtual ~FullscreenRendering_OpenGL();

		virtual void RenderFramebufferFullscreen(iFramebuffer& framebuffer, unsigned int screenWidth, unsigned int screenHeight) override;

	private:
		jle::gfx::Shader_OpenGL quadScreenShader;
		unsigned int quadVAO, quadVBO;
	};
}
