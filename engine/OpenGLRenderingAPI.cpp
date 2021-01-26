#include "OpenGLRenderingAPI.h"

#include "3rdparty/glad/glad.h"

void OpenGLRenderingAPI::SetViewportDimensions(int x, int y, unsigned int width, unsigned int height)
{
	glViewport(x, y, static_cast<int>(width), static_cast<int>(height));
}

void OpenGLRenderingAPI::Render()
{
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}