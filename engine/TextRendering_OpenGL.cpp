// Copyright (c) 2022. Johan Lind

#include "TextRendering_OpenGL.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#define __gl3_h_
#else

#include <glad/glad.h>

#endif

#define GLT_IMPLEMENTATION

#include <gltext/gltext.h>

jle::TextRendering_OpenGL::TextRendering_OpenGL() {
    gltInit();
}

jle::TextRendering_OpenGL::~TextRendering_OpenGL() {
    gltTerminate();
}

void jle::TextRendering_OpenGL::SendSimpleText(const std::string &text, float x, float y, float scale, float r, float g,
                                               float b, float a) {
    jleSimpleTextData td{x, y, scale, r, g, b, a};
    td.gltextPtr = gltCreateText();
    gltSetText(td.gltextPtr, text.c_str());
    mSimpleTextDatas.push_back(td);
}

void jle::TextRendering_OpenGL::Render(iFramebuffer &framebufferOut, const jleCamera &camera) {

    if (mSimpleTextDatas.empty() && mFontTextDatas.empty()) {
        return;
    }

    framebufferOut.BindToFramebuffer();

    if(!mFontTextDatas.empty())
    {
        // Render text with fonts
        jle::jleFont::SetRenderTargetDimensions(framebufferOut.GetWidth(), framebufferOut.GetHeight(), camera);
        for (auto &&textData: mFontTextDatas) {
            textData.font->RenderText(textData.text, textData.fontSize, textData.x, textData.y, textData.depth,
                                      {textData.r, textData.g, textData.b}, camera);
        }
    }


    if (!mSimpleTextDatas.empty()) {
        gltBeginDraw();

        for (auto &&textData: mSimpleTextDatas) {
            gltColor(textData.r, textData.g, textData.b, textData.a);
            gltDrawText2D(textData.gltextPtr, textData.x - camera.GetIntX(), textData.y - camera.GetIntY(), textData.scale);
        }

        gltEndDraw();
    }

    framebufferOut.BindToDefaultFramebuffer();
}

void jle::TextRendering_OpenGL::ClearBuffersForNextFrame() {
    // Clean up after rendering this frame
    for (auto &&textData: mSimpleTextDatas) {
        gltDeleteText(textData.gltextPtr);
    }
    mSimpleTextDatas.clear();
    mFontTextDatas.clear();
}

void jle::TextRendering_OpenGL::SendFontText(jle::jleFont *font, const std::string &text, uint32_t fontSize, float x,
                                             float y,
                                             float depth, float r, float g, float b, float a) {
    jleFontTextData data;
    mFontTextDatas.push_back({x, y, depth, r, g, b, a, fontSize, font, text});
}
