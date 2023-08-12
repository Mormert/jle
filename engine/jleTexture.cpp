// Copyright (c) 2023. Johan Lind

#include "jleTexture.h"

#include "stb_image.h"

#include "jleIncludeGL.h"

#include "jleGameEngine.h"
#include "jleImage.h"
#include <plog/Log.h>

#include <iostream>

jleTexture::~jleTexture()
{
    if (_id != UINT_MAX) {
        glDeleteTextures(1, &_id);
    }
}

jleLoadFromFileSuccessCode
jleTexture::loadFromFile(const jlePath &path)
{
    auto fe = path.getFileEnding();
    if (fe != "tex") {
        imagePath = path;
    }

    auto image = jleImage{imagePath};

    _width = image.width();
    _height = image.height();
    _nrChannels = image.nrChannels();

    glGenTextures(1, &_id);

    glBindTexture(GL_TEXTURE_2D, _id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (image.data()) {
        GLenum format = GL_RGBA;
        if (image.nrChannels() == 1)
            format = GL_RED;
        else if (image.nrChannels() == 2)
            format = GL_RED;
        else if (image.nrChannels() == 3)
            format = GL_RGB;
        else if (image.nrChannels() == 4)
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
            GL_TEXTURE_2D, 0, format, image.width(), image.height(), 0, format, GL_UNSIGNED_BYTE, image.data());
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        PLOG_VERBOSE << "Generated OpenGL texture (ID=" << _id << ") " << path.getVirtualPath() << " ("
                     << image.nrChannels() << " channels)";

        glBindTexture(GL_TEXTURE_2D, 0);

    } else {
        PLOG_ERROR << "Failed to generate OpenGL texture " << _id << " with path: " << path.getVirtualPath();

        return jleLoadFromFileSuccessCode::FAIL;
    }

    return jleLoadFromFileSuccessCode::SUCCESS;
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
std::vector<std::string>
jleTexture::getFileAssociationList()
{
    return {"png", "jpg", "tga", "bmp", "psd", "jpeg"};
}

const int
jleTexture::channels()
{
    return _nrChannels;
}
