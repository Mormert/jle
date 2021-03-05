#pragma once

#include "iFramebuffer.h"

namespace jle
{
	class Framebuffer_OpenGL : public iFramebuffer
	{
	public:

		Framebuffer_OpenGL(unsigned int width, unsigned int height);

		virtual ~Framebuffer_OpenGL();
		virtual void CreateFramebuffer(unsigned int width, unsigned int height) override;
		virtual void ResizeFramebuffer(unsigned int width, unsigned int height) override;

		virtual void BindToFramebuffer() override;
		virtual void BindToDefaultFramebuffer() override;

		virtual unsigned int GetWidth() override;
		virtual unsigned int GetHeight() override;

		virtual void* GetTexture() override;

	private:
		unsigned int width, height;
		unsigned int framebuffer;
		unsigned int texColorBuffer;
		unsigned int rbo;
	};
}