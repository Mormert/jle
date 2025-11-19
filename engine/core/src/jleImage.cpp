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

#include "jleImage.h"
#include "jleCommon.h"

#include <stb/stb_image.h>

#include <algorithm>

bool
jleImage::loadFromFile(jleSerializationContext &ctx, const jlePath &path)
{
    return loadImage(path);
}

bool
jleImage::loadImage(const jlePath &path)
{
    const auto &realPath = path.getRealPath();
    image_data = stbi_load(realPath.str().c_str(), &_width, &_height, &_nrChannels, 0);

    if (image_data) {
        return true;
    }
    return false;
}

jleImage::jleImage(const jlePath &path)
{
    if (!loadImage(path)) {
        LOGE << "Failed loading image with path: " << path.getVirtualPath().str();
    }
}

jleImage::jleImage(const jleImage &i)
{
    std::copy(i.image_data, i.image_data + i._nrChannels * i._width * i._height, this->image_data);
    this->_nrChannels = i._nrChannels;
    this->_width = i._height;
    this->_height = i._height;
}

jleImage &
jleImage::operator=(const jleImage &i)
{
    if (this == &i) {
        return *this;
    }
    if (image_data) {
        stbi_image_free(image_data);
    }

    std::copy(i.image_data, i.image_data + i._nrChannels * i._width * i._height, this->image_data);
    this->_nrChannels = i._nrChannels;
    this->_width = i._height;
    this->_height = i._height;

    return *this;
}

jleImage::jleImage(jleImage &&i) noexcept
{
    this->image_data = i.image_data;
    this->_nrChannels = i._nrChannels;
    this->_width = i._height;
    this->_height = i._height;

    i._height = 0;
    i._width = 0;
    i._nrChannels = 1;
    i.image_data = nullptr;
}

jleImage &
jleImage::operator=(jleImage &&i) noexcept
{
    if (this != &i) {
        if (image_data) {
            stbi_image_free(image_data);
        }

        this->image_data = i.image_data;
        this->_nrChannels = i._nrChannels;
        this->_width = i._height;
        this->_height = i._height;

        i._height = 0;
        i._width = 0;
        i._nrChannels = 1;
        i.image_data = nullptr;
    }
    return *this;
}

jleImage::~jleImage()
{
    if (image_data) {
        stbi_image_free(image_data);
    }
}

unsigned int
jleImage::height() const
{
    return _height;
}

unsigned int
jleImage::nrChannels() const
{
    return _nrChannels;
}

unsigned char *
jleImage::data() const
{
    return image_data;
}

unsigned int
jleImage::width() const
{
    return _width;
}

void
jleImage::setFlipImage(bool flip)
{
    stbi_set_flip_vertically_on_load(flip);
}
