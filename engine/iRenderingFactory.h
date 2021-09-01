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

		virtual std::unique_ptr<iRenderingAPI>			CreateRenderingAPI() const = 0;
		virtual std::unique_ptr<iQuadRendering>			CreateQuadRendering() const = 0;
		virtual std::unique_ptr<iTextRendering>			CreateTextRendering() const = 0;
		virtual std::unique_ptr<iFullscreenRendering>	CreateFullscreenRendering() const = 0;
		virtual std::unique_ptr<iFramebuffer>			CreateFramebuffer(unsigned int width, unsigned int height) const = 0;

	};
}