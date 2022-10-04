// Copyright (c) 2022. Johan Lind

#include "jleTextRendering.h"
#include "jleFont.h"
#include "jleFrameBuffer.h"
#include "jleProfiler.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#define __gl3_h_
#else

#include <glad/glad.h>

#endif

#define GLT_IMPLEMENTATION

#include <gltext/gltext.h>

jleTextRendering::jleTextRendering() { gltInit(); }

jleTextRendering::~jleTextRendering() { gltTerminate(); }

void jleTextRendering::sendSimpleText(const std::string &text,
                                      float x,
                                      float y,
                                      float scale,
                                      float r,
                                      float g,
                                      float b,
                                      float a) {
    jleSimpleTextData td{x, y, scale, r, g, b, a};
    td.gltextPtr = gltCreateText();
    gltSetText(td.gltextPtr, text.c_str());
    _simpleTextDatas.push_back(td);
}

void jleTextRendering::render(jleFramebuffer &framebufferOut,
                              const jleCamera &camera) {
    JLE_SCOPE_PROFILE(jleTextRendering::Render)
    if (_simpleTextDatas.empty() && _fontTextDatas.empty()) {
        return;
    }

    framebufferOut.bind();

    if (!_fontTextDatas.empty()) {
        // Render text with fonts
        jleFont::renderTargetDimensions(
            framebufferOut.width(), framebufferOut.height(), camera);
        for (auto &&textData : _fontTextDatas) {
            textData.font->renderText(textData.text,
                                      textData.fontSize,
                                      textData.x,
                                      textData.y,
                                      textData.depth,
                                      {textData.r, textData.g, textData.b},
                                      camera);
        }
    }

    if (!_simpleTextDatas.empty()) {
        gltBeginDraw();

        for (auto &&textData : _simpleTextDatas) {
            gltColor(textData.r, textData.g, textData.b, textData.a);
            gltDrawText2D(textData.gltextPtr,
                          textData.x - camera.intX(),
                          textData.y - camera.intY(),
                          textData.scale);
        }

        gltEndDraw();
    }

    framebufferOut.bindDefault();
}

void jleTextRendering::clearBuffersForNextFrame() {
    // Clean up after rendering this frame
    for (auto &&textData : _simpleTextDatas) {
        gltDeleteText(textData.gltextPtr);
    }
    _simpleTextDatas.clear();
    _fontTextDatas.clear();
}

void jleTextRendering::sendFontText(jleFont *font,
                                    const std::string &text,
                                    uint32_t fontSize,
                                    float x,
                                    float y,
                                    float depth,
                                    float r,
                                    float g,
                                    float b,
                                    float a) {
    jleFontTextData data;
    _fontTextDatas.push_back({x, y, depth, r, g, b, a, fontSize, font, text});
}
