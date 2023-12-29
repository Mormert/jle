/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#pragma once

#include "jleCamera.h"
#include <string>
#include <vector>

class jleFont;
class jleFramebufferInterface;

class jleTextRendering {
public:
    jleTextRendering();

    ~jleTextRendering();

    void sendFontText(jleFont *font,
                      const std::string &text,
                      uint32_t fontSize,
                      float x,
                      float y,
                      float depth,
                      float r,
                      float g,
                      float b,
                      float a);

    void render(jleFramebufferInterface &framebufferOut, const jleCamera &camera);

    void clearBuffersForNextFrame();

private:
    struct jleFontTextData {
        float x, y, depth, r, g, b, a;
        uint32_t fontSize;
        jleFont *font;
        std::string text;
    };
    std::vector<jleFontTextData> _fontTextDatas;
};
