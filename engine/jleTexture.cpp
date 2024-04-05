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

#include "jleTexture.h"
#include "jleGameEngine.h"
#include "jleImage.h"
#include "jleIncludeGL.h"
#include "jleRenderThread.h"

#include <plog/Log.h>
#include <stb_image.h>

#include <iostream>

jleTexture::jleTexture() {}

jleTexture::~jleTexture()
{
    if (_id != UINT_MAX) {
        glDeleteTextures(1, &_id);
    }
}

bool
jleTexture::loadFromFile(const jlePath &path)
{
    JLE_EXEC_IF(JLE_BUILD_HEADLESS) { return true; }

    auto fe = path.getFileEnding();
    if (fe != "tex") {
        imagePath = path;
    }

    _image = std::make_unique<jleImage>(imagePath);

    _width = _image->width();
    _height = _image->height();
    _nrChannels = _image->nrChannels();

    if (!_image->data()) {
        PLOG_ERROR << "Failed to generate OpenGL texture " << _id << " with path: " << path.getVirtualPath();
        return false;
    }

    gEngine->renderThread().runOnRenderThread([this, path]() {
        glGenTextures(1, &_id);

        glBindTexture(GL_TEXTURE_2D, _id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        GLenum format = GL_RGBA;
        if (_image->nrChannels() == 1)
            format = GL_RED;
        else if (_image->nrChannels() == 2)
            format = GL_RED;
        else if (_image->nrChannels() == 3)
            format = GL_RGB;
        else if (_image->nrChannels() == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, _id);
        if (format == GL_RGBA) {
            // Byte alignment 4 is defaulted for RGBA images
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        } else {
            // Needed to load images with different byte alignments
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }

        glTexImage2D(
            GL_TEXTURE_2D, 0, format, _image->width(), _image->height(), 0, format, GL_UNSIGNED_BYTE, _image->data());
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        PLOG_VERBOSE << "Generated OpenGL texture (ID=" << _id << ") " << path.getVirtualPath() << " ("
                     << _image->nrChannels() << " channels)";

        glBindTexture(GL_TEXTURE_2D, 0);
    });

    return true;
}

void
jleTexture::setActive(jleTextureSlot slot)
{
    glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
    glBindTexture(GL_TEXTURE_2D, _id);
}

int32_t
jleTexture::width()
{
    return _width;
}

int32_t
jleTexture::height()
{
    return _height;
}

unsigned int
jleTexture::id()
{
    return _id;
}

const int
jleTexture::channels()
{
    return _nrChannels;
}
