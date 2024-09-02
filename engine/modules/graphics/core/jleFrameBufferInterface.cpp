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

#include "jleFrameBufferInterface.h"

#include "jleIncludeGL.h"

#include "glm/glm.hpp"
#include <plog/Log.h>

#include <iostream>

jleFramebufferInterface::~jleFramebufferInterface()
{
    if (_framebuffer) {
        glDeleteFramebuffers(1, &_framebuffer);
    }
    std::cout << "Deleted Framebuffer with id " << _framebuffer << "!\n";
}

void
jleFramebufferInterface::blitToOther(jleFramebufferInterface &framebuffer, bool includeDepth)
{
    jleAssert(width() == framebuffer.width());
    jleAssert(height() == framebuffer.height());

    glBindFramebuffer(GL_READ_FRAMEBUFFER, _framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer._framebuffer);

    int flag;
    if (includeDepth) {
        flag = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    } else {
        flag = GL_COLOR_BUFFER_BIT;
    }

    glBlitFramebuffer(
        0, 0, (int)width(), (int)height(), 0, 0, (int)framebuffer.width(), (int)framebuffer.height(), flag, GL_LINEAR);
}

void
jleFramebufferInterface::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
}

void
jleFramebufferInterface::bindDefault()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int
jleFramebufferInterface::width() const
{
    return _width;
}

unsigned int
jleFramebufferInterface::height() const
{
    return _height;
}

unsigned int
jleFramebufferInterface::texture() const
{
    // Note: the framebuffer doesn't have to keep a texture!
    jleAssert(_texture > 0);
    return _texture;
}

glm::ivec2
jleFramebufferInterface::fixedAxisDimensions(jleFramebufferInterface::FIXED_AXIS fixedAxis,
                                             float aspect,
                                             unsigned int fixedAxisPixels)
{
    if (fixedAxis == FIXED_AXIS::height) {
        auto w = static_cast<unsigned int>((float)fixedAxisPixels * aspect);
        return {w, fixedAxisPixels};
    } else {
        auto h = static_cast<unsigned int>((float)fixedAxisPixels * aspect);
        return {fixedAxisPixels, h};
    }
}

jleFramebufferInterface::jleFramebufferInterface(unsigned int width, unsigned int height)
    : _width{width}, _height{height}
{
}