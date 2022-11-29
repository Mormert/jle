// Copyright (c) 2022. Johan Lind

#include "jleFramebufferScreen.h"
#include "jleIncludeGL.h"
#include "plog/Log.h"

jleFramebufferScreen::jleFramebufferScreen(unsigned int width, unsigned int height)
    : jleFramebufferInterface{width, height}
{

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    // Generate color texture
    glGenTextures(1, &_colorTexture);
    glBindTexture(GL_TEXTURE_2D, _colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)width, (int)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Attach the texture to the bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR << "Framebuffer is not complete!";
    } else {
        LOG_VERBOSE << "Created Framebuffer with id " << _framebuffer << "!";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

jleFramebufferScreen::~jleFramebufferScreen()
{
    if (_colorTexture) {
        glDeleteTextures(1, &_colorTexture);
    }
}

void
jleFramebufferScreen::resize(unsigned int width, unsigned int height)
{
    this->_width = width;
    this->_height = height;

    glBindTexture(GL_TEXTURE_2D, _colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)width, (int)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
}