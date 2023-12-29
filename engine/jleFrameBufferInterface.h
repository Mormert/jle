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

#include <glm/glm.hpp>
#include <memory>

class jleFramebufferInterface
{
public:
    jleFramebufferInterface(unsigned int width, unsigned int height);

    virtual ~jleFramebufferInterface();

    // Copy framebuffer content from this framebuffer to another
    void blitToOther(jleFramebufferInterface &framebuffer, bool includeDepth = false);

    virtual void resize(unsigned int width, unsigned int height) = 0;

    // Specifies which axis to be fixed, and the other scales depending on window aspect ratio
    enum class FIXED_AXIS { width, height };

    static glm::ivec2 fixedAxisDimensions(jleFramebufferInterface::FIXED_AXIS fixedAxis,
                                          float aspect,
                                          unsigned int fixedAxisPixels);
    void bind();

    static void bindDefault();

    [[nodiscard]] unsigned int width() const;

    [[nodiscard]] unsigned int height() const;

    [[nodiscard]] unsigned int texture() const;

    static std::shared_ptr<jleFramebufferInterface> GetMainScreenFramebuffer();

protected:
    unsigned int _width{}, _height{};

    // The framebuffer object
    unsigned int _framebuffer{};

    // The color texture.
    // Note: It can be 0 and unset, depending on framebuffer type.
    unsigned int _texture{};
};
