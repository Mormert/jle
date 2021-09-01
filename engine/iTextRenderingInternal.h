#pragma once

#include "iTextRendering.h"

#include "iFramebuffer.h"

namespace jle
{
	class iTextRenderingInternal : public iTextRendering
	{
	public:
		virtual ~iTextRenderingInternal() {}

		// Renders to an output framebuffer
		virtual void Render(iFramebuffer& framebufferOut) = 0;
	};
}