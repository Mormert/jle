// Copyright (c) 2022. Johan Lind

#include "jleFramebufferShadowMap.h"

#include "jleIncludeGL.h"
#include "plog/Log.h"

jleFramebufferShadowMap::jleFramebufferShadowMap(unsigned int width, unsigned int height)
    : jleFramebufferInterface(width, height)
{

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    // Generate texture
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);

    // Attach the texture to the bound framebuffer object as GL_DEPTH_COMPONENT
#ifdef BUILD_OPENGLES30
    // OpenGL ES 3.0 uses 16 bit depth component here instead of default 24 bit
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT16,
                 (int)width,
                 (int)height,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_UNSIGNED_SHORT,
                 NULL);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

#ifdef __EMSCRIPTEN__
    // Not implemented ...
    // WebGL doesn't have GL_CLAMP_TO_BORDER and GL_TEXTURE_BORDER_COLOR
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_OES);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_OES);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR_OES, borderColor);
#endif

    // Render depth values to the texture (GL_DEPTH_ATTACHMENT)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _texture, 0);

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
jleFramebufferShadowMap::resize(unsigned int width, unsigned int height)
{
    this->_width = width;
    this->_height = height;

    glBindTexture(GL_TEXTURE_2D, _texture);
#ifdef BUILD_OPENGLES30
    // OpenGL ES 3.0 uses 16 bit depth component here instead of default 24 bit
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT16,
                 (int)width,
                 (int)height,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_UNSIGNED_SHORT,
                 NULL);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
#endif
    glBindTexture(GL_TEXTURE_2D, 0);
}

jleFramebufferShadowMap::~jleFramebufferShadowMap()
{
    if (_texture) {
        glDeleteTextures(1, &_texture);
    }
}
