#pragma once
#include "iRenderingFactory.h"

namespace jle
{
	class RenderingFactory_OpenGL : public iRenderingFactory
	{
	public:
		virtual std::unique_ptr<iRenderingAPI>			CreateRenderingAPI() const override;
		virtual std::unique_ptr<iQuadRendering>			CreateQuadRendering() const override;
		virtual std::unique_ptr<iTextRendering>			CreateTextRendering() const override;
		virtual std::unique_ptr<iFramebuffer>			CreateFramebuffer(unsigned int width, unsigned int height) const override;
		virtual std::unique_ptr<iFullscreenRendering>	CreateFullscreenRendering() const override;
	};
}