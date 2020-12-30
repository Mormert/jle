#pragma once
#include "Framebuffer.h"

#include "Shader.h"

namespace jle
{
	namespace gfx
	{
		class ScreenFramebuffer : public Framebuffer
		{
		public:
			using Framebuffer::Framebuffer;

			ScreenFramebuffer(unsigned int width, unsigned int height);

			virtual ~ScreenFramebuffer() override;

			// Note: Changes viewport dimensions
			void PrepareForSceneRender();

			// Note: Changes viewport dimensions to window dimensions
			void RenderToScreen();

		private:
			jle::internals::Shader quadScreenShader;
			unsigned int quadVAO, quadVBO;
		};

	}
}