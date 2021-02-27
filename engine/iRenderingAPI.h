#pragma once

#include "iQuadRendering.h"

#include <memory>

namespace jle
{
	class iRenderingAPI
	{
	public:
		virtual ~iRenderingAPI() {};

		std::unique_ptr<iQuadRendering> quads;
	};
}