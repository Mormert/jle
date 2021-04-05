#include "EditorBackgroundImage.h"

namespace jle
{
	EditorBackgroundImage::EditorBackgroundImage(
		std::shared_ptr<iTexture> texture, 
		std::unique_ptr<iFramebuffer>& framebuffer,
		std::unique_ptr<iFullscreenRendering>& fullscreen_renderer) :
		background_texture { texture },
		background_framebuffer { std::move(framebuffer) }, 
		background_quad{ background_texture },
		background_fullscreen_renderer {std::move(fullscreen_renderer)}
	{	
		background_quad.width = texture->GetWidth();
		background_quad.height = texture->GetHeight();
		
	
	}

	void EditorBackgroundImage::Render(iQuadRenderingInternal& quadRenderer, unsigned int width, unsigned int height)
	{
		quadRenderer.SendTexturedQuad(background_quad, RenderingMethod::Dynamic);
		quadRenderer.Render(*background_framebuffer);

		background_fullscreen_renderer->RenderFramebufferFullscreen(*background_framebuffer, width, height);
	}
}