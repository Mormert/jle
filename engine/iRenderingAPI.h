#pragma once

#include "iQuadRendering.h"
#include "iTextRendering.h"

#include <memory>

namespace jle
{
	class iRenderingAPI
	{
	public:
		virtual ~iRenderingAPI() {};

		std::unique_ptr<iQuadRendering> quads;
		std::unique_ptr<iTextRendering> texts;
	};
}