#pragma once

#include "iTextureCreator.h"

namespace jle
{
	class TextureCreator_OpenGL final : public iTextureCreator
	{
	public:
		virtual ~TextureCreator_OpenGL() {}

		virtual std::unique_ptr<iTexture> CreateTextureFromImage(const Image& image) override;
	};
}