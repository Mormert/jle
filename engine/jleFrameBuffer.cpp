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
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &texColorBuffer);

    std::cout << "Deleted Framebuffer with id " << framebuffer << "!\n";
}

void jleFramebuffer::CreateFramebuffer(unsigned int width,
                                       unsigned int height) {
    this->width = width;
    this->height = height;

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // generate texture
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
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
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR << "Framebuffer is not complete!";
    }
    else {
        LOG_VERBOSE << "Created Framebuffer with id " << framebuffer << "!";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void jleFramebuffer::ResizeFramebuffer(unsigned int width,
                                       unsigned int height) {

    // LOG_VERBOSE << "Resized Framebuffer " << framebuffer << ": " << width <<
    // ", " << height;

    this->width = width;
    this->height = height;

    // resize texture
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    jleStaticOpenGLState::globalActiveTexture = texColorBuffer;
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
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void jleFramebuffer::BindToFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void jleFramebuffer::BindToDefaultFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int jleFramebuffer::GetWidth() { return width; }

unsigned int jleFramebuffer::GetHeight() { return height; }

unsigned int jleFramebuffer::GetTexture() { return texColorBuffer; }

void jleFramebuffer::CreateShadowFramebuffer(unsigned int width,
                                             unsigned int height) {
    this->width = width;
    this->height = height;

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // generate texture
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
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

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texColorBuffer, 0);
    glDrawBuffers(GL_NONE, nullptr);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
