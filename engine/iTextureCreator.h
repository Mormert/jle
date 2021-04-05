#pragma once

#include "iTexture.h"
#include "Image.h"

#include <memory>

namespace jle
{
	class iTextureCreator
	{
	public:
		virtual ~iTextureCreator() {}
		virtual std::unique_ptr<iTexture> CreateTextureFromImage(const Image& image) = 0;
	};
}