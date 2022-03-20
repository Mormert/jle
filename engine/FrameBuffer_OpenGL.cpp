#include "FrameBuffer_OpenGL.h"

#include "3rdparty/glad/glad.h"

#include "GLState.h"
#include "plog/Log.h"

#include <iostream>

namespace jle
{
	Framebuffer_OpenGL::Framebuffer_OpenGL(unsigned int width, unsigned int height)
	{
		CreateFramebuffer(width, height);
	}

	Framebuffer_OpenGL::~Framebuffer_OpenGL()
	{
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteRenderbuffers(1, &rbo);
		glDeleteTextures(1, &texColorBuffer);

		std::cout << "Deleted Framebuffer with id " << framebuffer << "!\n";
	}

	void Framebuffer_OpenGL::CreateFramebuffer(unsigned int width, unsigned int height)
	{
		this->width = width;
		this->height = height;

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		// generate texture
		glGenTextures(1, &texColorBuffer);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

	void Framebuffer_OpenGL::ResizeFramebuffer(unsigned int width, unsigned int height)
	{

        LOG_VERBOSE << "Resized Framebuffer " << framebuffer << ": " << width << ", " << height;

		this->width = width;
		this->height = height;

		// resize texture
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        jle::GLState::globalActiveTexture = texColorBuffer;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);

		// resize renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void Framebuffer_OpenGL::BindToFramebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	}

	void Framebuffer_OpenGL::BindToDefaultFramebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int Framebuffer_OpenGL::GetWidth()
	{
		return width;
	}

	unsigned int Framebuffer_OpenGL::GetHeight()
	{
		return height;
	}

    unsigned int Framebuffer_OpenGL::GetTexture()
	{
        return texColorBuffer;
	}



}

