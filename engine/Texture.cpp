#include "Texture.h"

#include "3rdparty/glad/glad.h"

#include "GLStateMachine.h"

#include <iostream>

namespace jle
{
	namespace graphics
	{

		Texture::Texture(std::string texturePath)
			: texturePath{ texturePath } {}


		int Texture::GetHeight()
		{
			return height;
		}

		int Texture::GetWidth()
		{
			return width;
		}

		Texture::~Texture()
		{
			if (texture_id != UINT_MAX)
			{
				glDeleteTextures(1, &texture_id);
			}

			std::cout << "Destroyed texture " << texture_id << ".\n";
		}

		bool Texture::IsActive()
		{
			//std::cout << globalActiveTexture << " " << texture_id << "\n";
			return gfx::glStateMachine.globalActiveTexture == texture_id;
		}

		void Texture::SetToActiveTexture()
		{
			if (!isCreated)
			{
				glGenTextures(1, &texture_id);
				glBindTexture(GL_TEXTURE_2D, texture_id);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				// set texture filtering parameters
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				stbi_set_flip_vertically_on_load(false);

				unsigned char* textureData = stbi_load(texturePath.c_str(),
					&width, &height, &nrChannels, 0);


				if (textureData)
				{
					GLenum format = GL_RGBA;
					if (nrChannels == 1)
						format = GL_RED;
					else if (nrChannels == 3)
						format = GL_RGB;
					else if (nrChannels == 4)
						format = GL_RGBA;

					glBindTexture(GL_TEXTURE_2D, texture_id);
					glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
					glGenerateMipmap(GL_TEXTURE_2D);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

					std::cout << "Generated texture " << texture_id << ".\n";

					stbi_image_free(textureData);
				}
				else
				{
					std::cout << "Texture failed to load at path: " << texturePath << std::endl;
					stbi_image_free(textureData);
				}

				glBindTexture(GL_TEXTURE_2D, 0);

				isCreated = true;
			}

			glBindTexture(GL_TEXTURE_2D, texture_id);
			glActiveTexture(GL_TEXTURE0);
			gfx::glStateMachine.globalActiveTexture = texture_id;
		}
	}
}