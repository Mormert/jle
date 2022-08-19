// Copyright (c) 2022. Johan Lind

#include "Texture.h"

#include "3rdparty/stb_image.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <glad/glad.h>

#endif

#include "jleStaticOpenGLState.h"

#include <iostream>

namespace jle {
    Texture::~Texture() {
        if (texture_id != UINT_MAX) {
            glDeleteTextures(1, &texture_id);
        }
        std::cout << "Destroyed texture with id " << texture_id << '\n';
    }

    bool Texture::IsActive() {
        return jle::jleStaticOpenGLState::globalActiveTexture == texture_id;
    }

    void Texture::SetToActiveTexture(int texture_slot) {
        glActiveTexture(GL_TEXTURE0 + texture_slot);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        jle::jleStaticOpenGLState::globalActiveTexture = texture_id;
    }

    unsigned int Texture::GetTextureID() {
        return texture_id;
    }

}