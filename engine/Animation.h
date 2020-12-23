#pragma once

#include "Sprite.h"
#include <vector>
#include <string>

namespace jle
{
	namespace graphics
	{
		class Animation
		{
			//Animation(std::string animationDataPath, float frameTime);
		public:
			void DrawAnimation(int worldX, int worldY, float depth, unsigned int frame);

			void InsertFrame(const Sprite& frame);

		private:

			std::vector<Sprite> frames;
		};
	}
}