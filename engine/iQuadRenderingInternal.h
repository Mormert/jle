#pragma once

#include "iQuadRendering.h"

#include "iFramebuffer.h"

namespace jle
{
	class iQuadRenderingInternal : public iQuadRendering
	{

	public:
		virtual ~iQuadRenderingInternal() {}

		// Renders to an output framebuffer
		virtual void Render(iFramebuffer& framebufferOut) = 0;
	};
}