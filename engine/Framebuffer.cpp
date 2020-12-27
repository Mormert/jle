#include "Framebuffer.h"

#include "3rdparty/glad/glad.h"

#include <iostream>

// Much code based from learnopengl.com

namespace jle
{
	namespace gfx {
		
		Framebuffer::Framebuffer(unsigned int width, unsigned int height) : width{width}, height{height}
		{
			glGenFramebuffers(1, &framebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

			// generate texture
			glGenTextures(1, &texColorBuffer);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);

			// attach it to currently bound framebuffer object
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				std::cerr << "Error: Framebuffer is not complete!\n";
			}
			else
			{
				std::cout << "Created Framebuffer with id " << framebuffer << "!\n";
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
				
		}

		Framebuffer::~Framebuffer()
		{
			glDeleteFramebuffers(1, &framebuffer);
			glDeleteRenderbuffers(1, &rbo);
			glDeleteTextures(1, &texColorBuffer);
		}

		void Framebuffer::Use()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		}


		void Framebuffer::UseDefaultFramebuffer()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		
	}
}


