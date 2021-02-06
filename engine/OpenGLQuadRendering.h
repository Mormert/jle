#pragma once

#include "iQuadRenderingInternal.h"

#include "Shader.h"

#include <vector>

class OpenGLQuadRendering : public iQuadRenderingInternal
{
public:

	OpenGLQuadRendering();

	virtual ~OpenGLQuadRendering();

	// Inherited via iQuadRenderingInternal
	virtual void SendTexturedQuad(TexturedQuad& texturedQuad, RenderingMethod renderingMethod) override;
	virtual void SendColoredQuad(ColoredQuad& coloredQuad, RenderingMethod renderingMethod) override;
	virtual void Render(iFramebuffer& framebufferOut) override;

private:
	jle::internals::Shader quadShader;
	unsigned int quadVBO, quadVAO;

	std::vector<TexturedQuad> texturedQuads;
};

