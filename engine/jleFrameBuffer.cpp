// Copyright (c) 2022. Johan Lind

#include "jleFrameBuffer.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <glad/glad.h>

#endif

#include "glm/glm.hpp"
#include "jleStaticOpenGLState.h"
#include "plog/Log.h"

#include <iostream>

jleFramebuffer::jleFramebuffer(unsigned int width, unsigned int height, bool shadowBuffer)
{
    if (shadowBuffer) {
        createShadowMappingFramebuffer(width, height);
    } else {
        createRenderFramebuffer(width, height);
    }
}

jleFramebuffer::~jleFramebuffer()
{
    glDeleteFramebuffers(1, &_framebuffer);
    if (_rbo) {
        glDeleteRenderbuffers(1, &_rbo);
    }
    glDeleteTextures(1, &_texColorBuffer);

    std::cout << "Deleted Framebuffer with id " << _framebuffer << "!\n";
}

void
jleFramebuffer::createRenderFramebuffer(unsigned int width, unsigned int height)
{
    this->_width = width;
    this->_height = height;

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    // Generate color texture
    glGenTextures(1, &_texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, _texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Attach the texture to the bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texColorBuffer, 0);

    // We also need a render buffer object for depth testing, which we create here
    glGenRenderbuffers(1, &_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Attach the depth and stencil attachment to the bound framebuffer object
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR << "Framebuffer is not complete!";
    } else {
        LOG_VERBOSE << "Created Framebuffer with id " << _framebuffer << "!";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
jleFramebuffer::createShadowMappingFramebuffer(unsigned int width, unsigned int height)
{

    _isShadowFramebuffer = true;

    this->_width = width;
    this->_height = height;

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    // Generate texture
    glGenTextures(1, &_texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, _texColorBuffer);

    // Attach the texture to the bound framebuffer object as GL_DEPTH_COMPONENT
#ifdef BUILD_OPENGLES30
    // OpenGL ES 3.0 uses 16 bit depth component here instead of default 24 bit
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_OES);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_OES);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR_OES, borderColor);

    // Render depth values to the texture (GL_DEPTH_ATTACHMENT)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _texColorBuffer, 0);

    // We only need depth information when rendering the scene from a light's perspective.
    // Thus, a shadow mapping framebuffer does not have a color buffer.
    // We need however, to tell OpenGL that we won't have such a buffer, and we do this by
    // setting the Draw and Read buffers to GL_NONE:
    glDrawBuffers(GL_NONE, GL_NONE);
    glReadBuffer(GL_NONE);

    // Make sure that the framebuffer is complete. It will get black when rendered onto otherwise.
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOGE << "Error building shadow framebuffer!";
    } else {
        LOG_VERBOSE << "Created Shadow Framebuffer with id " << _framebuffer << "!";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
jleFramebuffer::resize(unsigned int width, unsigned int height)
{
    this->_width = width;
    this->_height = height;

    // resize texture
    glBindTexture(GL_TEXTURE_2D, _texColorBuffer);
    jleStaticOpenGLState::globalActiveTexture = _texColorBuffer;
    if (_isShadowFramebuffer) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // resize renderbuffer, if this framebuffer uses one
    if (_rbo) {
        glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
}

void
jleFramebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
}

void
jleFramebuffer::bindDefault()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    jleStaticOpenGLState::globalActiveTexture = 0;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int
jleFramebuffer::width()
{
    return _width;
}

unsigned int
jleFramebuffer::height()
{
    return _height;
}

unsigned int
jleFramebuffer::texture()
{
    return _texColorBuffer;
}

glm::ivec2
jleFramebuffer::fixedAxisDimensions(jleFramebuffer::FIXED_AXIS fixedAxis, float aspect, unsigned int fixedAxisPixels)
{
    if (fixedAxis == FIXED_AXIS::height) {
        auto w = static_cast<unsigned int>((float)fixedAxisPixels * aspect);
        return {w, fixedAxisPixels};
    } else {
        auto h = static_cast<unsigned int>((float)fixedAxisPixels * aspect);
        return {fixedAxisPixels, h};
    }
}
