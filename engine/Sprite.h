#pragma once

#include <memory>

#include "Renderer2D.h"
#include "Texture.h"

namespace jle
{
	namespace graphics
	{
		class Sprite
		{
		public:

			Sprite(Texture& texture, int x, int y, int width, int height);

			void DrawSprite(int worldX, int worldY, float depth) const;

		private:
			Texture& texture;

			// Positions on the sprite sheet
			int x, y, width, height;
		};
	}
}