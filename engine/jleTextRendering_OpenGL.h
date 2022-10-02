// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleCamera.h"
#include <string>
#include <vector>

struct GLTtext;
class jleFont;
class jleFramebufferInterface;

class jleTextRendering_OpenGL {
public:
    jleTextRendering_OpenGL();

    ~jleTextRendering_OpenGL();

    void SendSimpleText(const std::string& text,
                        float x,
                        float y,
                        float scale,
                        float r,
                        float g,
                        float b,
                        float a);

    void SendFontText(jleFont *font,
                      const std::string& text,
                      uint32_t fontSize,
                      float x,
                      float y,
                      float depth,
                      float r,
                      float g,
                      float b,
                      float a);

    void Render(jleFramebufferInterface& framebufferOut,
                const jleCamera& camera);

    void ClearBuffersForNextFrame();

private:
    struct jleSimpleTextData {
        float x, y, scale, r, g, b, a;
        GLTtext *gltextPtr;
    };
    std::vector<jleSimpleTextData> _simpleTextDatas;

    struct jleFontTextData {
        float x, y, depth, r, g, b, a;
        uint32_t fontSize;
        jleFont *font;
        std::string text;
    };
    std::vector<jleFontTextData> _fontTextDatas;
};
