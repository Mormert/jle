#pragma once

#include "iQuadRenderingInternal.h"

#include "Shader_OpenGL.h"

#include <vector>

namespace jle
{
	class QuadRendering_OpenGL : public iQuadRenderingInternal
	{
	public:

		QuadRendering_OpenGL();

		virtual ~QuadRendering_OpenGL();

		// Inherited via iQuadRenderingInternal
		virtual void SendTexturedQuad(TexturedQuad& texturedQuad, RenderingMethod renderingMethod) override;
		virtual void SendColoredQuad(ColoredQuad& coloredQuad, RenderingMethod renderingMethod) override;
		virtual void Render(iFramebuffer& framebufferOut, const jleCamera& camera) override;

        virtual void ClearBuffersForNextFrame();

	private:
		jle::gfx::Shader_OpenGL quadShader;
		unsigned int quadVBO, quadVAO;

		// Instanced version
		jle::gfx::Shader_OpenGL quadShaderInstanced;
		unsigned int quadVBO_Instanced, quadVAO_Instanced, instanceVBO, elementbuffer;

		std::vector<TexturedQuad> texturedQuads;
	};
}



