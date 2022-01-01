#include "Texture.h"

#include "3rdparty/stb_image.h"

#include "3rdparty/glad/glad.h"

#include "GLState.h"

#include "plog/Log.h"

#include <iostream>

namespace jle
{
	Texture::~Texture()
	{
		if (texture_id != UINT_MAX)
		{
			glDeleteTextures(1, &texture_id);
		}
		PLOG_VERBOSE << "Destroyed texture " << texture_id << ".\n";
		std::cout << "Destroyed texture\n";
	}

	bool Texture::IsActive()
	{
		return glStaticState.globalActiveTexture == texture_id;
	}

	void Texture::SetToActiveTexture()
	{
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glActiveTexture(GL_TEXTURE0);
		glStaticState.globalActiveTexture = texture_id;
	}

    unsigned int Texture::GetTextureID() {
        return texture_id;
    }

}