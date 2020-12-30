#pragma once

#include "Texture.h"
#include "Shader.h"


#include "Viewport.h"
#include "PixelQuadRenderer.h"
#include "ScreenFramebuffer.h"


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

		int GetMouseWorldX() const noexcept;
		int GetMouseWorldY() const noexcept;

		struct {
			int xPos = 0, yPos = 0, width = 240, height = 135;
		} camera;

		PixelQuadRenderer pixelQuadRenderer;

	private:
		std::unique_ptr<gfx::ScreenFramebuffer> screenFramebuffer;

		glm::mat4 cameraMat{ 1.0f };
		glm::mat4& GetCameraMat();


		// Called by engine
		Renderer2D(Viewport& vp);

		Viewport& viewport;

		void Render();

		void SetAspectRatio(unsigned int w, unsigned int h);

		static Renderer2D* mainRenderer;

		friend class Engine;

	};
}
