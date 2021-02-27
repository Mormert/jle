#pragma once

#include "iRenderingAPI.h"
#include "iFramebuffer.h"
#include "iQuadRendering.h"
#include "iFullscreenRendering.h"

#include <memory>

namespace jle
{
	class iRenderingFactory
	{
	public:
		virtual ~iRenderingFactory() {}

		virtual std::unique_ptr<iRenderingAPI> CreateRenderingAPI() = 0;
		virtual std::unique_ptr<iQuadRendering> CreateQuadRendering() = 0;
		virtual std::unique_ptr<iFullscreenRendering> CreateFullscreenRendering() = 0;


		virtual std::unique_ptr<iFramebuffer> CreateFramebuffer(unsigned int width, unsigned int height) = 0;

	};
}