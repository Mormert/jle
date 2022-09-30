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

#include "jleStaticOpenGLState.h"

#include <iostream>

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
