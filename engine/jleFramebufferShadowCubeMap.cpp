// Copyright (c) 2022. Johan Lind

#include "jleFramebufferShadowCubeMap.h"

#include "jleIncludeGL.h"
#include "plog/Log.h"

jleFramebufferShadowCubeMap::jleFramebufferShadowCubeMap(unsigned int width, unsigned int height)
    : jleFramebufferInterface(width, height)
{

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_DEPTH_COMPONENT16,
                     (int)width,
                     (int)height,
                     0,
                     GL_DEPTH_COMPONENT,
                     GL_UNSIGNED_SHORT,
                     NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Attach the 0th cube texture face to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 0, _texture, 0);

    // Don't use any color attachment:
    glDrawBuffers(GL_NONE, GL_NONE);
    glReadBuffer(GL_NONE);

    // Make sure that the framebuffer is complete.
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOGE << "Error building shadow framebuffer!";
    } else {
        LOG_VERBOSE << "Created Shadow Framebuffer with id " << _framebuffer << "!";
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
jleFramebufferShadowCubeMap::resize(unsigned int width, unsigned int height)
{
    this->_width = width;
    this->_height = height;

    glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_DEPTH_COMPONENT,
                     (int)width,
                     (int)height,
                     0,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     NULL);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

jleFramebufferShadowCubeMap::~jleFramebufferShadowCubeMap()
{
    if (_texture) {
        glDeleteTextures(1, &_texture);
    }
}
void
jleFramebufferShadowCubeMap::setRenderFace(unsigned int index)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, _texture, 0);
}
