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

#include "jleBuildConfig.h"

#include "jleFrameBufferInterface.h"

// A framebuffer that supports anti-aliasing (MSAA)
class jleFramebufferMultisample : public jleFramebufferInterface
{
public:
    jleFramebufferMultisample(unsigned int width, unsigned int height, unsigned int msaaSamples);

    ~jleFramebufferMultisample() override;

    void resize(unsigned int width, unsigned int height) override;

    void setSamples(unsigned int samples);

private:
    unsigned int _colorRenderbuffer{};
    unsigned int _depthRenderbuffer{};

    unsigned int _msaaSamples{4};
};
