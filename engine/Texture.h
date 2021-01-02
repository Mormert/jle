#pragma once

#include <string>
#include "3rdparty/stb_image.h"

namespace jle
{
	namespace graphics
	{
		class Texture
		{
		public:
			// Created by specifying the path to the image that the
			// texture will be based on
			Texture(std::string texturePath);
			virtual ~Texture();

			// no copy, no move
			Texture(const Texture& t) = delete;
			Texture& operator= (const Texture& t) = delete;
			Texture(Texture&& t) = delete;
			Texture& operator=(Texture&& t) = delete;

			// Returns true if this Texture is the globally active texture
			bool IsActive();
			// Set this Texture to be the globally active texture
			void SetToActiveTexture();

			// Returns the width of the texture
			int GetWidth();
			// Returns the height of the texture
			int GetHeight();

		private:

			const std::string texturePath;
			bool isCreated = false;

			int width = 0, height = 0, nrChannels = 0;
			unsigned int texture_id = UINT_MAX; // OpenGL Texture ID

		};
	}
}