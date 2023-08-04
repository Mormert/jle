// Copyright (c) 2023. Johan Lind

#include "jleFramebufferPicking.h"

#include "jleIncludeGL.h"
#include <plog/Log.h>

jleFramebufferPicking::jleFramebufferPicking(unsigned int width, unsigned int height)
    : jleFramebufferInterface(width, height)
{
    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    // Generate color texture
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)width, (int)height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Attach the texture to the bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

    // We also need a render buffer object for depth testing, which we create here
    glGenRenderbuffers(1, &_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)width, (int)height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Attach the depth and stencil attachment to the bound framebuffer object
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthRenderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR << "Framebuffer is not complete!";
    } else {
        LOG_VERBOSE << "Created Framebuffer with id " << _framebuffer << "!";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

jleFramebufferPicking::~jleFramebufferPicking()
{
    if (_texture) {
        glDeleteTextures(1, &_texture);
    }
}

void
jleFramebufferPicking::resize(unsigned int width, unsigned int height)
{
    this->_width = width;
    this->_height = height;

    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)width, (int)height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)width, (int)height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
