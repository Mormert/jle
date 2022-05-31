#pragma once

#include "iTextRendering.h"
#include "jleCamera.h"
#include "iFramebuffer.h"

namespace jle
{
	class iTextRenderingInternal : public iTextRendering
	{
	public:
		virtual ~iTextRenderingInternal() {}

		// Renders to an output framebuffer
		virtual void Render(iFramebuffer& framebufferOut, const jleCamera& camera) = 0;
        virtual void ClearBuffersForNextFrame() = 0;
	};
}