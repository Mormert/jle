#pragma once

#include "iQuadRenderingInternal.h"

#include "Shader_OpenGL.h"

#include <vector>

class QuadRendering_OpenGL : public iQuadRenderingInternal
{
public:

	QuadRendering_OpenGL();

	virtual ~QuadRendering_OpenGL();

	// Inherited via iQuadRenderingInternal
	virtual void SendTexturedQuad(TexturedQuad& texturedQuad, RenderingMethod renderingMethod) override;
	virtual void SendColoredQuad(ColoredQuad& coloredQuad, RenderingMethod renderingMethod) override;
	virtual void Render(iFramebuffer& framebufferOut) override;

private:
	jle::gfx::Shader_OpenGL quadShader;
	unsigned int quadVBO, quadVAO;

	std::vector<TexturedQuad> texturedQuads;
};

