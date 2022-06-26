// Copyright (c) 2022. Johan Lind

#pragma once

#include "iTextRenderingInternal.h"

#include <vector>

struct GLTtext;

namespace jle {
    class TextRendering_OpenGL : public iTextRenderingInternal {
    public:

        TextRendering_OpenGL();

        ~TextRendering_OpenGL() override;

        void SendSimpleText(const std::string &text, float x, float y, float scale, float r, float g, float b,
                            float a) override;

        void
        SendFontText(jle::jleFont *font, const std::string &text, uint32_t fontSize, float x, float y, float depth, float r,
                     float g, float b, float a) override;


        void Render(iFramebuffer &framebufferOut, const jleCamera &camera) override;

        void ClearBuffersForNextFrame() override;

    private:
        struct jleSimpleTextData {
            float x, y, scale, r, g, b, a;
            GLTtext *gltextPtr;
        };
        std::vector<jleSimpleTextData> mSimpleTextDatas;

        struct jleFontTextData {
            float x, y, depth, r, g, b, a;
            uint32_t fontSize;
            jleFont *font;
            std::string text;
        };
        std::vector<jleFontTextData> mFontTextDatas;
    };
}