#pragma once

namespace jle
{
	class iFramebuffer
	{
	public:
		virtual ~iFramebuffer() {}

		virtual void CreateFramebuffer(unsigned int width, unsigned int height) = 0;
		virtual void ResizeFramebuffer(unsigned int width, unsigned int height) = 0;

		virtual void BindToFramebuffer() = 0;
		virtual void BindToDefaultFramebuffer() = 0;

		virtual unsigned int GetWidth() = 0;
		virtual unsigned int GetHeight() = 0;

		virtual void* GetTexture() = 0;
	};
}

