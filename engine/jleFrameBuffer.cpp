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

#include "jleStaticOpenGLState.h"
#include "plog/Log.h"

#include <iostream>

jleFramebuffer::jleFramebuffer(unsigned int width,
                               unsigned int height,
                               bool shadowBuffer) {
    if (shadowBuffer) {
        CreateShadowFramebuffer(width, height);
    }
    else {
        CreateFramebuffer(width, height);
    }
}

jleFramebuffer::~jleFramebuffer() {
    glDeleteFramebuffers(1, &_framebuffer);
    glDeleteRenderbuffers(1, &_rbo);
    glDeleteTextures(1, &_texColorBuffer);

    std::cout << "Deleted Framebuffer with id " << _framebuffer << "!\n";
}

void jleFramebuffer::CreateFramebuffer(unsigned int width,
                                       unsigned int height) {
    this->_width = width;
    this->_height = height;

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    // generate texture
    glGenTextures(1, &_texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, _texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 width,
                 height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texColorBuffer, 0);

    glGenRenderbuffers(1, &_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR << "Framebuffer is not complete!";
    }
    else {
        LOG_VERBOSE << "Created Framebuffer with id " << _framebuffer << "!";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void jleFramebuffer::ResizeFramebuffer(unsigned int width,
                                       unsigned int height) {

    // LOG_VERBOSE << "Resized Framebuffer " << framebuffer << ": " << width <<
    // ", " << height;

    this->_width = width;
    this->_height = height;

    // resize texture
    glBindTexture(GL_TEXTURE_2D, _texColorBuffer);
    jleStaticOpenGLState::globalActiveTexture = _texColorBuffer;
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 width,
                 height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    // resize renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void jleFramebuffer::BindToFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
}

void jleFramebuffer::BindToDefaultFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int jleFramebuffer::GetWidth() { return _width; }

unsigned int jleFramebuffer::GetHeight() { return _height; }

unsigned int jleFramebuffer::GetTexture() { return _texColorBuffer; }

void jleFramebuffer::CreateShadowFramebuffer(unsigned int width,
                                             unsigned int height) {
    this->_width = width;
    this->_height = height;

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    // generate texture
    glGenTextures(1, &_texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, _texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT,
                 width,
                 height,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _texColorBuffer, 0);
    glDrawBuffers(GL_NONE, nullptr);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
