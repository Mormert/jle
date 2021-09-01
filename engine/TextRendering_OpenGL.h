#pragma once

#include "iTextRenderingInternal.h"

#include <vector>

struct GLTtext;

namespace jle
{
	class TextRendering_OpenGL : public iTextRenderingInternal
	{
	public:

		TextRendering_OpenGL();

		virtual ~TextRendering_OpenGL();

		virtual void SendSimpleText(const std::string& text, float x, float y, float scale, float r, float g, float b, float a) override;
		virtual void Render(iFramebuffer& framebufferOut) override;

	private:
		struct TextData
		{
			float x, y, scale, r, g, b, a;
			GLTtext* gltextPtr;
		};
		std::vector<TextData> mTextDatas;
	};
}