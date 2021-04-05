#pragma once

#include "iTexture.h"
#include "iFramebuffer.h"
#include "iFullscreenRendering.h"
#include "iQuadRenderingInternal.h"

#include <memory>

namespace jle
{
	class EditorBackgroundImage final
	{
	public:
		EditorBackgroundImage(
			std::shared_ptr<iTexture> texture,
			std::unique_ptr<iFramebuffer>& framebuffer,
			std::unique_ptr<iFullscreenRendering>& fullscreen_renderer);

		void Render(iQuadRenderingInternal& quadRenderer, unsigned int width, unsigned int height);
	private:
		std::shared_ptr<iTexture> background_texture;
		std::unique_ptr<iFramebuffer> background_framebuffer;
		std::unique_ptr<iFullscreenRendering> background_fullscreen_renderer;
		TexturedQuad background_quad;
	};
}