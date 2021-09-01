#include "RenderingAPI_OpenGL.h"

#include "3rdparty/glad/glad.h"

#include "iQuadRenderingInternal.h"
#include "iTextRenderingInternal.h"

namespace jle
{
	void RenderingAPI_OpenGL::SetViewportDimensions(int x, int y, unsigned int width, unsigned int height)
	{
		glViewport(x, y, static_cast<int>(width), static_cast<int>(height));
	}

	void RenderingAPI_OpenGL::Render(iFramebuffer& framebufferOut)
	{
		((iQuadRenderingInternal*)quads.get())->Render(framebufferOut);
		((iTextRenderingInternal*)texts.get())->Render(framebufferOut);
	}

	void RenderingAPI_OpenGL::Setup(const iRenderingFactory& renderFactory)
	{
		this->quads = renderFactory.CreateQuadRendering();
		this->texts = renderFactory.CreateTextRendering();
	}
}


