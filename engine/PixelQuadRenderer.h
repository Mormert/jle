#pragma once

#include "Texture.h"
#include "Shader.h"

#include <glm/glm.hpp>

#include <vector>

namespace jle
{
	class PixelQuadRenderer
	{
	public:

		PixelQuadRenderer(const PixelQuadRenderer& p) = delete;
		PixelQuadRenderer& operator= (const PixelQuadRenderer& p) = delete;
		PixelQuadRenderer(PixelQuadRenderer&& p) = delete;
		PixelQuadRenderer& operator=(PixelQuadRenderer&& p) = delete;

		// Sends a textured pixel quad to-be-rendered on Render() called from Renderer2D
		void SendTexturedPixelQuadDynamic(int worldX, int worldY, float depth, graphics::Texture& texture, int x, int y, int width, int height);

	private:

		friend class Renderer2D;

		PixelQuadRenderer();
		virtual ~PixelQuadRenderer();

		const static unsigned char quadIndices[];
		const static std::string quadVertexShaderSrc;
		const static std::string quadFragShaderSrc;
		internals::Shader quadShader;

		// OpenGL buffer IDs
		unsigned int quadVBO, quadVAO;

		// Renders out all previously send textured pixel quads,
		// called from Renderer2D.
		void Render(const glm::mat4& cameraMat);

		// Contains all data necessary to render a textured pixel quad
		struct TexturedPixelQuad {
			int worldX, worldY;
			float depth;
			graphics::Texture& texture;
			int x, y, width, height;
		};

		// Contains this frame's textured pixel quads to be rendered
		// The vector is cleared after all have been rendered
		std::vector<TexturedPixelQuad> texturedPixelQuadsDynamic;
	};
}



