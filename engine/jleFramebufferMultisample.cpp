// Copyright (c) 2023. Johan Lind

#include "jleFramebufferMultisample.h"
#include <plog/Log.h>

#include "jleIncludeGL.h"

jleFramebufferMultisample::jleFramebufferMultisample(unsigned int width, unsigned int height, unsigned int msaaSamples)
    : jleFramebufferInterface(width, height)
{
    _msaaSamples = msaaSamples;

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    // Generate color renderbuffer
    glGenRenderbuffers(1, &_colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderbuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, (int)_msaaSamples, GL_RGBA8, (int)width, (int)height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderbuffer);

    // Alternative method: Generate multisample color texture
    // Caveat: Only works on desktop OpenGL
    /* glGenTextures(1, &_texColorBuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _texColorBuffer);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, (int)samples, GL_RGB, (int)width, (int)height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    // Attach the texture to the bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _texColorBuffer, 0); */

    // Generate depth buffer
    // We also need a render buffer object for depth testing.
    glGenRenderbuffers(1, &_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderbuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, (int)_msaaSamples, GL_DEPTH_COMPONENT24, (int)width, (int)height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Attach the depth and stencil attachment to the bound framebuffer object
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR << "Framebuffer is not complete!";
    } else {
        LOG_VERBOSE << "Created Framebuffer with id " << _framebuffer << "!";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

jleFramebufferMultisample::~jleFramebufferMultisample()
{
    if (_colorRenderbuffer) {
        glDeleteRenderbuffers(1, &_colorRenderbuffer);
    }
    if (_depthRenderbuffer) {
        glDeleteRenderbuffers(1, &_depthRenderbuffer);
    }
}

void
jleFramebufferMultisample::resize(unsigned int width, unsigned int height)
{
    this->_width = width;
    this->_height = height;

    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderbuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, (int)_msaaSamples, GL_RGBA8, (int)width, (int)height);

    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderbuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, (int)_msaaSamples, GL_DEPTH_COMPONENT24, (int)width, (int)height);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
void
jleFramebufferMultisample::setSamples(unsigned int samples)
{
    int highestSamples;
    glGetIntegerv(GL_SAMPLES, &highestSamples);
    assert(samples < highestSamples);

    _msaaSamples = samples;
    resize(_width, _height);
}
