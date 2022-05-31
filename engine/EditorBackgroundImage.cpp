#include "EditorBackgroundImage.h"

namespace jle
{
	EditorBackgroundImage::EditorBackgroundImage(const Image& image, iTextureCreator& tc, iRenderingFactory& rf) :
		background_framebuffer { rf.CreateFramebuffer(image.GetImageWidth(), image.GetImageHeight()) },
		background_fullscreen_renderer { rf.CreateFullscreenRendering() },
		background_texture { tc.CreateTextureFromImage(image) },
		background_quad { background_texture }
	{	
		background_quad.width = image.GetImageWidth();
		background_quad.height = image.GetImageHeight();
	}

	void EditorBackgroundImage::Render(iQuadRenderingInternal& quadRenderer, unsigned int width, unsigned int height)
	{
		quadRenderer.SendTexturedQuad(background_quad, RenderingMethod::Dynamic);
		quadRenderer.Render(*background_framebuffer, jleCamera{});
        quadRenderer.ClearBuffersForNextFrame();

		background_fullscreen_renderer->RenderFramebufferFullscreen(*background_framebuffer, width, height);
	}
}