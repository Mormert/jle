#pragma once

#include "iTextureCreator.h"

namespace jle
{
	class TextureCreator_OpenGL final : public iTextureCreator
	{
	public:
		virtual std::unique_ptr<iTexture> CreateTextureFromImage(const Image& image) override;
	};
}