// Copyright (c) 2022. Johan Lind

#include "jleFont.h"
#include "jleStaticOpenGLState.h"

#include "jlePathDefines.h"
#include "glm/ext/matrix_clip_space.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <glad/glad.h>

#endif

#include <plog/Log.h>

#include <cassert>
#include <iostream>

jle::jleFont::jleFont(const std::string &path) {
    LoadFromFile(path);
}

jle::jleFont::~jleFont() {
    if (sInitialized && mFontLoaded) {
        FT_Done_Face(mFace);
    }
}

void jle::jleFont::Init() {
    if (sInitialized) {
        return;
    }

    if (FT_Init_FreeType(&sFreeTypeLibrary)) {
        LOGE << "Could not init FreeType Library!";
        std::exit(EXIT_FAILURE);
    }

    sShader = std::make_unique<gfx::Shader_OpenGL>(std::string{JLE_ENGINE_PATH_SHADERS + "/font.vert"}.c_str(),
                                                   std::string{JLE_ENGINE_PATH_SHADERS + "/font.frag"}.c_str());

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(300), 0.0f, static_cast<float>(400));
    sShader->Use();
    sShader->SetMat4("projection", projection);

    glGenVertexArrays(1, &sVAO);
    glGenBuffers(1, &sVBO);
    glBindVertexArray(sVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    sInitialized = true;
}

void jle::jleFont::DeInit() {
    if (sFreeTypeLibrary != nullptr) {
        FT_Done_FreeType(sFreeTypeLibrary);
    }
    sFreeTypeLibrary = nullptr;
    sInitialized = false;
}

void jle::jleFont::RenderText(const std::string &text, uint32_t fontSize,
                              float x, float y, float depth, glm::vec3 color,
                              const jle::jleCamera &camera) {

    // TODO: Add scale as param
    constexpr float scale = 1.f;

    auto it = mFontSizeLookup.find(fontSize);
    if (it == mFontSizeLookup.end()) {
        // Font Size not setup, so set it up
        AddFontSizePixels(fontSize);
    }

    auto fontSz = mFontSizeLookup[fontSize];

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sShader->Use();
    sShader->SetVec3("textColor", color.x, color.y, color.z);
    sShader->SetFloat("depth", depth);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(sVAO);

    const auto ySizeOffset = mFace->size->metrics.y_ppem;

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        jleFontSize::jleFontCharacter ch = fontSz.mCharacters[*c];

        float xpos = x + ch.mBearing.x * scale;
        float ypos = y + ySizeOffset - ch.mSize.y + (ch.mSize.y - ch.mBearing.y); // TODO: Add scale multiplier

        float w = ch.mSize.x * scale;
        float h = ch.mSize.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
                {xpos,     ypos + h, 0.0f, 1.0f},
                {xpos + w, ypos,     1.0f, 0.0f},
                {xpos,     ypos,     0.0f, 0.0f},

                {xpos + w, ypos,     1.0f, 0.0f},
                {xpos,     ypos + h, 0.0f, 1.0f},
                {xpos + w, ypos + h, 1.0f, 1.0f}
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.mTextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, sVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.mAdvance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    jle::jleStaticOpenGLState::globalActiveTexture = 0;
}

void jle::jleFont::SetRenderTargetDimensions(int width, int height, const jleCamera &camera) {

    sProj = glm::ortho(static_cast<float>(camera.GetIntX()),
                       static_cast<float>(camera.GetIntX() + width),
                       static_cast<float>(camera.GetIntY() + height),
                       static_cast<float>(camera.GetIntY()), -1.f, 1.f);

    sShader->Use();
    sShader->SetMat4("projection", sProj);

    glViewport(0, 0, width, height);
}

bool jle::jleFont::LoadFromFile(const std::string &path) {
    if (!sInitialized) {
        Init();
    }

    if (FT_New_Face(sFreeTypeLibrary, path.c_str(), 0, &mFace)) {
        LOGE << "Failed to load font: " << path;
        return false;
    }

    mFontLoaded = true;
    return true;
}

void jle::jleFont::AddFontSizePixels(uint32_t sizePixels) {
    FT_Set_Pixel_Sizes(mFace, 0, sizePixels);

    // Check if it already contains a setup for the given pixel size
    if (mFontSizeLookup.count(sizePixels)) {
        return;
    }

    jleFontSize fontSize;

    // Disable byte alignment in OpenGL
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        // load character glyph
        if (FT_Load_Char(mFace, c, FT_LOAD_RENDER)) {
            LOGW << "FreeType failed to load " << c;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        jleStaticOpenGLState::globalActiveTexture = texture;

#ifdef BUILD_OPENGLES30 // Use ALPHA as format on GL ES instead of the RED component
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_ALPHA,
                mFace->glyph->bitmap.width,
                mFace->glyph->bitmap.rows,
                0,
                GL_ALPHA,
                GL_UNSIGNED_BYTE,
                mFace->glyph->bitmap.buffer
        );
#else
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                mFace->glyph->bitmap.width,
                mFace->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                mFace->glyph->bitmap.buffer
        );
#endif

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use

        jleFontSize::jleFontCharacter character = {
                texture,
                glm::ivec2(mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows),
                glm::ivec2(mFace->glyph->bitmap_left, mFace->glyph->bitmap_top),
                static_cast<unsigned int>(mFace->glyph->advance.x)
        };
        fontSize.mCharacters.insert(std::pair<char, jleFontSize::jleFontCharacter>(c, character));
    }

    mFontSizeLookup[sizePixels] = fontSize;
}
