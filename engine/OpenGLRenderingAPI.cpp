#include "OpenGLRenderingAPI.h"

#include "3rdparty/glad/glad.h"

#include "iQuadRenderingInternal.h"

void OpenGLRenderingAPI::SetViewportDimensions(int x, int y, unsigned int width, unsigned int height)
{
	glViewport(x, y, static_cast<int>(width), static_cast<int>(height));
}

void OpenGLRenderingAPI::Render(iFramebuffer& framebufferOut, unsigned int width, unsigned int height)
{
	((iQuadRenderingInternal*)quads.get())->Render(framebufferOut);
}

void OpenGLRenderingAPI::Setup(std::unique_ptr<iQuadRendering> quads)
{
	this->quads = std::move(quads);
}
