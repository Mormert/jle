// Copyright (c) 2022. Johan Lind

#include "jleFont.h"
#include "jleStaticOpenGLState.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "jlePathDefines.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <glad/glad.h>

#endif

#include <plog/Log.h>

#include <cassert>
#include <iostream>

jleFont::jleFont(const std::string &path) { loadFromFile(path); }

jleFont::~jleFont() {
    if (jleFontData::data && _fontLoaded) {
        FT_Done_Face(_face);
    }
}

void jleFont::renderText(const std::string &text,
                         uint32_t fontSize,
                         float x,
                         float y,
                         float depth,
                         glm::vec3 color,
                         const jleCamera &camera) {

    if (!_fontLoaded) {
        return;
    }

    // TODO: Add scale as param
    constexpr float scale = 1.f;

    auto it = _fontSizeLookup.find(fontSize);
    if (it == _fontSizeLookup.end()) {
        // Font Size not setup, so set it up
        addFontSizePixels(fontSize);
    }

    auto fontSz = _fontSizeLookup[fontSize];

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    jleFontData::data->shader->use();
    jleFontData::data->shader->SetVec3("textColor", color.x, color.y, color.z);
    jleFontData::data->shader->SetFloat("depth", depth);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(jleFontData::data->vao);

    const auto ySizeOffset = _face->size->metrics.y_ppem;

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        jleFontSize::jleFontCharacter ch = fontSz._characters[*c];

        float xpos = x + ch._bearing.x * scale;
        float ypos = y + ySizeOffset - ch._size.y +
                     (ch._size.y - ch._bearing.y); // TODO: Add scale multiplier

        float w = ch._size.x * scale;
        float h = ch._size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {{xpos, ypos + h, 0.0f, 1.0f},
                                {xpos + w, ypos, 1.0f, 0.0f},
                                {xpos, ypos, 0.0f, 0.0f},

                                {xpos + w, ypos, 1.0f, 0.0f},
                                {xpos, ypos + h, 0.0f, 1.0f},
                                {xpos + w, ypos + h, 1.0f, 1.0f}};

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch._textureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, jleFontData::data->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of
        // 1/64 pixels)
        x += (ch._advance >> 6) *
             scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    jleStaticOpenGLState::globalActiveTexture = 0;
}

void jleFont::renderTargetDimensions(int width,
                                     int height,
                                     const jleCamera &camera) {

    sProj = camera.getProjectionMatrix();

    jleFontData::data->shader->use();
    jleFontData::data->shader->SetMat4("projection", sProj);

    glViewport(0, 0, width, height);
}

bool jleFont::loadFromFile(const std::string &path) {
    if (!jleFontData::data) {
        throw std::runtime_error{"font not loaded"};
    }

    if (FT_New_Face(
            jleFontData::data->freeTypeLibrary, path.c_str(), 0, &_face)) {
        LOGE << "Failed to load font: " << path;
        return false;
    }

    _fontLoaded = true;
    return true;
}

void jleFont::addFontSizePixels(uint32_t sizePixels) {
    FT_Set_Pixel_Sizes(_face, 0, sizePixels);

    // Check if it already contains a setup for the given pixel size
    if (_fontSizeLookup.count(sizePixels)) {
        return;
    }

    jleFontSize fontSize;

    // Disable byte alignment in OpenGL
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        // load character glyph
        if (FT_Load_Char(_face, c, FT_LOAD_RENDER)) {
            LOGW << "FreeType failed to load " << c;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        jleStaticOpenGLState::globalActiveTexture = texture;

#ifdef BUILD_OPENGLES30 // Use ALPHA as format on GL ES instead of the RED
                        // component
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_ALPHA,
                     _face->glyph->bitmap.width,
                     _face->glyph->bitmap.rows,
                     0,
                     GL_ALPHA,
                     GL_UNSIGNED_BYTE,
                     _face->glyph->bitmap.buffer);
#else
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     _face->glyph->bitmap.width,
                     _face->glyph->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     _face->glyph->bitmap.buffer);
#endif

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use

        jleFontSize::jleFontCharacter character = {
            texture,
            glm::ivec2(_face->glyph->bitmap.width, _face->glyph->bitmap.rows),
            glm::ivec2(_face->glyph->bitmap_left, _face->glyph->bitmap_top),
            static_cast<unsigned int>(_face->glyph->advance.x)};
        fontSize._characters.insert(
            std::pair<char, jleFontSize::jleFontCharacter>(c, character));
    }

    _fontSizeLookup[sizePixels] = fontSize;
}

jleFontData::jleFontData() {
    if (data) {
        return;
    }

    data = this;

    if (FT_Init_FreeType(&freeTypeLibrary)) {
        LOGE << "Could not init FreeType Library!";
        std::exit(EXIT_FAILURE);
    }

    shader = std::make_unique<jleShader>(
        std::string{JLE_ENGINE_PATH_SHADERS + "/font.vert"}.c_str(),
        std::string{JLE_ENGINE_PATH_SHADERS + "/font.frag"}.c_str());

    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(300), 0.0f, static_cast<float>(400));
    shader->use();
    shader->SetMat4("projection", projection);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
jleFontData::~jleFontData() {

    if (freeTypeLibrary != nullptr) {
        FT_Done_FreeType(freeTypeLibrary);
    }
    freeTypeLibrary = nullptr;
    data = nullptr;
}
