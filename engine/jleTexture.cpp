// Copyright (c) 2022. Johan Lind

#include "jleTexture.h"

#include "3rdparty/stb_image.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <glad/glad.h>

#endif

#include "jleImage.h"
#include "jleStaticOpenGLState.h"
#include "plog/Log.h"

#include <iostream>
#include <memory>

jleTexture::~jleTexture() {
    if (texture_id != UINT_MAX) {
        glDeleteTextures(1, &texture_id);
    }
    std::cout << "Destroyed texture with id " << texture_id << '\n';
}

bool jleTexture::IsActive() {
    return jleStaticOpenGLState::globalActiveTexture == texture_id;
}

void jleTexture::SetToActiveTexture(int texture_slot) {
    glActiveTexture(GL_TEXTURE0 + texture_slot);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    jleStaticOpenGLState::globalActiveTexture = texture_id;
}

unsigned int jleTexture::GetTextureID() { return texture_id; }

int32_t jleTexture::GetWidth() { return width; }

int32_t jleTexture::GetHeight() { return height; }

std::unique_ptr<jleTexture> FromImage(const jleImage& image) {
    auto texture_opengl = std::make_unique<jleTexture>();

    texture_opengl->width = image.GetImageWidth();
    texture_opengl->height = image.GetImageHeight();
    texture_opengl->nrChannels = image.GetImageNrChannels();

    glGenTextures(1, &texture_opengl->texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_opengl->texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (image.GetImageData()) {
        GLenum format = GL_RGBA;
        if (image.GetImageNrChannels() == 1)
            format = GL_RED;
        else if (image.GetImageNrChannels() == 3)
            format = GL_RGB;
        else if (image.GetImageNrChannels() == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture_opengl->texture_id);
        if (format == GL_RGB) {
            // Needed to load jpg images with different byte alignments
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }
        else if (format == GL_RGBA) {
            // Byte alignment 4 is defaulted for RGBA images
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        }
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     format,
                     image.GetImageWidth(),
                     image.GetImageHeight(),
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     image.GetImageData());
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_S,
                        format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_T,
                        format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        PLOG_VERBOSE << "Generated OpenGL texture "
                     << texture_opengl->texture_id << " ("
                     << image.GetImageNrChannels() << " channels)";
    }
    else {
        PLOG_ERROR << "Failed to generate OpenGL texture "
                   << texture_opengl->texture_id
                   << " with path: " << image.GetPath();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    jleStaticOpenGLState::globalActiveTexture = 0;

    return texture_opengl;
}
