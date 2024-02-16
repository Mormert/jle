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

#include "jleShader.h"
#include "jleResourceRef.h"

class jleFramebufferInterface;

class jleFullscreenRendering {
public:
    jleFullscreenRendering(const jleFullscreenRendering &) = delete;
    jleFullscreenRendering &operator=(const jleFullscreenRendering &) = delete;
    jleFullscreenRendering(jleFullscreenRendering &&e) = delete;
    jleFullscreenRendering &operator=(jleFullscreenRendering &&e) = delete;

    jleFullscreenRendering();

    ~jleFullscreenRendering();

    void renderFramebufferFullscreen(jleFramebufferInterface &framebuffer,
                                     unsigned int screenWidth,
                                     unsigned int screenHeight);

private:
    jleResourceRef<jleShader> quadScreenShader;
    unsigned int quadVAO, quadVBO;
};
