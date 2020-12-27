#pragma once

namespace jle
{
	namespace gfx {
		class Framebuffer
		{
		public:
			Framebuffer(unsigned int width, unsigned int height);

			Framebuffer(const Framebuffer& f) = delete;
			Framebuffer& operator=(const Framebuffer& f) = delete;
			Framebuffer(Framebuffer&& f) = delete;
			Framebuffer& operator=(Framebuffer&& f) = delete;

			virtual ~Framebuffer();

			void Use();								// Note, need to call glViewport() after if dimensions change
			static void UseDefaultFramebuffer();	// Note, need to call glViewport() after if dimensions change

		protected:
			unsigned int width, height;
			unsigned int framebuffer;
			unsigned int texColorBuffer;
			unsigned int rbo;
		};
	}
}


