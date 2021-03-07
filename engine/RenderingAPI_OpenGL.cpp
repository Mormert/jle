#include "RenderingAPI_OpenGL.h"

#include "3rdparty/glad/glad.h"

#include "iQuadRenderingInternal.h"

namespace jle
{
	void RenderingAPI_OpenGL::SetViewportDimensions(int x, int y, unsigned int width, unsigned int height)
	{
		glViewport(x, y, static_cast<int>(width), static_cast<int>(height));
	}

	void RenderingAPI_OpenGL::Render(iFramebuffer& framebufferOut)
	{
		((iQuadRenderingInternal*)quads.get())->Render(framebufferOut);
	}

	void RenderingAPI_OpenGL::Setup(std::unique_ptr<iQuadRendering> quads)
	{
		this->quads = std::move(quads);
	}
}


