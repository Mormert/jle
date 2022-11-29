// Copyright (c) 2022. Johan Lind

#include "jleTextRendering.h"
#include "jleFont.h"
#include "jleFrameBufferInterface.h"
#include "jleProfiler.h"

jleTextRendering::jleTextRendering() {}

jleTextRendering::~jleTextRendering() {}

void
jleTextRendering::render(jleFramebufferInterface &framebufferOut, const jleCamera &camera)
{
    JLE_SCOPE_PROFILE(jleTextRendering::Render)

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

    framebufferOut.bindDefault();
}

void jleTextRendering::clearBuffersForNextFrame() {
    // Clean up after rendering this frame
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
