// Copyright (c) 2022. Johan Lind

#include "jleTextRendering_OpenGL.h"
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
    mSimpleTextDatas.push_back(td);
}

void jleTextRendering_OpenGL::Render(jleFramebufferInterface& framebufferOut,
                                     const jleCamera& camera) {
    JLE_SCOPE_PROFILE(jleTextRendering_OpenGL::Render)
    if (mSimpleTextDatas.empty() && mFontTextDatas.empty()) {
        return;
    }

    framebufferOut.BindToFramebuffer();

    if (!mFontTextDatas.empty()) {
        // Render text with fonts
        jleFont::SetRenderTargetDimensions(
            framebufferOut.GetWidth(), framebufferOut.GetHeight(), camera);
        for (auto&& textData : mFontTextDatas) {
            textData.font->RenderText(textData.text,
                                      textData.fontSize,
                                      textData.x,
                                      textData.y,
                                      textData.depth,
                                      {textData.r, textData.g, textData.b},
                                      camera);
        }
    }

    if (!mSimpleTextDatas.empty()) {
        gltBeginDraw();

        for (auto&& textData : mSimpleTextDatas) {
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
    for (auto&& textData : mSimpleTextDatas) {
        gltDeleteText(textData.gltextPtr);
    }
    mSimpleTextDatas.clear();
    mFontTextDatas.clear();
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
    mFontTextDatas.push_back({x, y, depth, r, g, b, a, fontSize, font, text});
}