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

        void Render(iFramebuffer &framebufferOut, const jleCamera &camera) override;

        void ClearBuffersForNextFrame() override;

    private:
        struct TextData {
            float x, y, scale, r, g, b, a;
            GLTtext *gltextPtr;
        };
        std::vector<TextData> mTextDatas;
    };
}