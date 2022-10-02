// Copyright (c) 2022. Johan Lind

#include "jleTextRendering_OpenGL.h"
#include "jleFont.h"
#include "jleFramebufferInterface.h"
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

jleTextRendering_OpenGL::jleTextRendering_OpenGL() { gltInit(); }

jleTextRendering_OpenGL::~jleTextRendering_OpenGL() { gltTerminate(); }

void jleTextRendering_OpenGL::SendSimpleText(const std::string& text,
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

void jleTextRendering_OpenGL::Render(jleFramebufferInterface& framebufferOut,
                                     const jleCamera& camera) {
    JLE_SCOPE_PROFILE(jleTextRendering_OpenGL::Render)
    if (_simpleTextDatas.empty() && _fontTextDatas.empty()) {
        return;
    }

    framebufferOut.BindToFramebuffer();

    if (!_fontTextDatas.empty()) {
        // Render text with fonts
        jleFont::SetRenderTargetDimensions(
            framebufferOut.GetWidth(), framebufferOut.GetHeight(), camera);
        for (auto&& textData : _fontTextDatas) {
            textData.font->RenderText(textData.text,
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

        for (auto&& textData : _simpleTextDatas) {
            gltColor(textData.r, textData.g, textData.b, textData.a);
            gltDrawText2D(textData.gltextPtr,
                          textData.x - camera.GetIntX(),
                          textData.y - camera.GetIntY(),
                          textData.scale);
        }

        gltEndDraw();
    }

    framebufferOut.BindToDefaultFramebuffer();
}

void jleTextRendering_OpenGL::ClearBuffersForNextFrame() {
    // Clean up after rendering this frame
    for (auto&& textData : _simpleTextDatas) {
        gltDeleteText(textData.gltextPtr);
    }
    _simpleTextDatas.clear();
    _fontTextDatas.clear();
}

void jleTextRendering_OpenGL::SendFontText(jleFont *font,
                                           const std::string& text,
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
