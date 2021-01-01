#pragma once

#include "Texture.h"
#include "Shader.h"

#include "Camera2D.h"
#include "PixelQuadRenderer.h"


#include <memory>
#include <map>
#include <glm/glm.hpp>

namespace jle
{
	class Renderer2D final
	{
	public:

		Renderer2D(const Renderer2D& r) = delete;
		Renderer2D& operator= (const Renderer2D& r) = delete;
		Renderer2D(Renderer2D&& r) = delete;
		Renderer2D& operator=(Renderer2D&& r) = delete;

		void RenderQuadTexture(int worldX, int worldY, float depth, graphics::Texture& texture, int x, int y, int width, int height);

		static Renderer2D& GetMainRenderer() { return *mainRenderer; };

		PixelQuadRenderer pixelQuadRenderer;

	private:


		// Called by engine
		Renderer2D();

		void Render(Camera2D& cam);

		static Renderer2D* mainRenderer;

		friend class Engine;

	};
}
