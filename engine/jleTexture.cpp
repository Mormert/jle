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
#include "jleResourceHolder.h"
#include "jleStaticOpenGLState.h"
#include "plog/Log.h"
#include <iostream>
#include <memory>

jleTexture::~jleTexture() {
    if (_id != UINT_MAX) {
        glDeleteTextures(1, &_id);
    }
    std::cout << "Destroyed texture with id " << _id << '\n';
}

bool jleTexture::IsActive() {
    return jleStaticOpenGLState::globalActiveTexture == _id;
}

void jleTexture::SetToActiveTexture(int texture_slot) {
    glActiveTexture(GL_TEXTURE0 + texture_slot);
    glBindTexture(GL_TEXTURE_2D, _id);
    jleStaticOpenGLState::globalActiveTexture = _id;
}

unsigned int jleTexture::GetTextureID() { return _id; }

int32_t jleTexture::GetWidth() { return _width; }

int32_t jleTexture::GetHeight() { return _height; }

unsigned int jleTexture::id() { return _id; }

jleTexture::jleTexture(const jleImage& image) {
    _width = image.GetImageWidth();
    _height = image.GetImageHeight();
    _nrChannels = image.GetImageNrChannels();

    glGenTextures(1, &_id);

    glBindTexture(GL_TEXTURE_2D, _id);

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

        glBindTexture(GL_TEXTURE_2D, _id);
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

        PLOG_VERBOSE << "Generated OpenGL texture " << _id << " ("
                     << image.GetImageNrChannels() << " channels)";
    }
    else {
        PLOG_ERROR << "Failed to generate OpenGL texture " << _id
                   << " with path: " << image.GetPath();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    jleStaticOpenGLState::globalActiveTexture = 0;
}

std::shared_ptr<jleTexture> jleTexture::FromPath(const jleRelativePath& path) {
    if (!jleResourceHolder::IsResourceLoaded(path)) {

        auto texture = std::make_shared<jleTexture>(
            *jleResourceHolder::LoadResourceFromFile<jleImage>(path));

        jleResourceHolder::StoreResource<jleTexture>(texture, path);

        return texture;
    }
    else {
        return jleResourceHolder::GetResource<jleTexture>(path);
    }
}
