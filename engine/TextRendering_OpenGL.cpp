#include "TextRendering_OpenGL.h"

#include "glad/glad.h"
#define GLT_IMPLEMENTATION
#include "3rdparty/gltext.h"

jle::TextRendering_OpenGL::TextRendering_OpenGL()
{
	gltInit();
}

jle::TextRendering_OpenGL::~TextRendering_OpenGL()
{
	gltTerminate();
}

void jle::TextRendering_OpenGL::SendSimpleText(const std::string& text, float x, float y, float scale, float r, float g, float b, float a)
{
	TextData td{ x, y , scale , r, g, b, a };
	td.gltextPtr = gltCreateText();
	gltSetText(td.gltextPtr, text.c_str());
	mTextDatas.push_back(td);
}

void jle::TextRendering_OpenGL::Render(iFramebuffer& framebufferOut)
{
	if (mTextDatas.size() == 0)
	{
		return;
	}

	framebufferOut.BindToFramebuffer();

	gltBeginDraw();

	for (auto&& textData : mTextDatas)
	{
		gltColor(textData.r, textData.g, textData.b, textData.a);
		gltDrawText2D(textData.gltextPtr, textData.x, textData.y, textData.scale);
	}

	gltEndDraw();

	// Clean up after rendering this frame
	for (auto&& textData : mTextDatas)
	{
		gltDeleteText(textData.gltextPtr);
	}
	mTextDatas.clear();

	framebufferOut.BindToDefaultFramebuffer();
}