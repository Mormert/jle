// Copyright (c) 2022. Johan Lind

#include "jleTexture.h"

#include "stb_image.h"

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

bool jleTexture::isActive() {
    return jleStaticOpenGLState::globalActiveTexture == _id;
}

void jleTexture::toActiveTexture(int texture_slot) {
    glActiveTexture(GL_TEXTURE0 + texture_slot);
    glBindTexture(GL_TEXTURE_2D, _id);
    jleStaticOpenGLState::globalActiveTexture = _id;
}

int32_t jleTexture::width() { return _width; }

int32_t jleTexture::height() { return _height; }

unsigned int jleTexture::id() { return _id; }

jleTexture::jleTexture(const jleImage& image) {
    _width = image.imageWidth();
    _height = image.imageHeight();
    _nrChannels = image.imageNrChannels();

    glGenTextures(1, &_id);

    glBindTexture(GL_TEXTURE_2D, _id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (image.imageData()) {
        GLenum format = GL_RGBA;
        if (image.imageNrChannels() == 1)
            format = GL_RED;
        else if (image.imageNrChannels() == 3)
            format = GL_RGB;
        else if (image.imageNrChannels() == 4)
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
                     image.imageWidth(),
                     image.imageHeight(),
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     image.imageData());
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
                     << image.imageNrChannels() << " channels)";
    }
    else {
        PLOG_ERROR << "Failed to generate OpenGL texture " << _id
                   << " with path: " << image.path();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    jleStaticOpenGLState::globalActiveTexture = 0;
}

std::shared_ptr<jleTexture> jleTexture::fromPath(const jleRelativePath& path) {
    if (!jleResourceHolder::isResourceLoaded(path)) {

        auto texture = std::make_shared<jleTexture>(
            *jleResourceHolder::loadResourceFromFile<jleImage>(path));

        jleResourceHolder::storeResource<jleTexture>(texture, path);

        return texture;
    }
    else {
        return jleResourceHolder::resource<jleTexture>(path);
    }
}
